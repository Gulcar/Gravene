#include "Server.h"

#include "NetCommon.h"
#include <iostream>
#include <fmt/core.h>
#include <fmt/color.h>
using namespace std::placeholders;

void Server::Start(uint16_t port)
{
    m_netServer.SetClientConnectedCallback([this](Net::Connection& conn) {
        AddConnection(conn);
    });
    m_netServer.SetClientDisconnectedCallback([this](Net::Connection& conn) {
        RemoveConnection(conn);
    });

    m_netServer.SetDataReceiveCallback(std::bind(&Server::DataReceive, this, _1, _2, _3, _4));

    m_netServer.Start(port);
    fmt::print("Listening on port {}\n", port);
}

void Server::Update()
{
    m_netServer.Process();

    UpdateClientPositions();
    UpdateBullets();
    UpdateCollisions();
    UpdateReviveTime();
    UpdatePowerUps();
}

void Server::UpdateClientPositions()
{
    // poslji vsak drug tick
    static bool send = true;
    send = !send;
    if (send == false)
        return;

    if (m_connections.size() == 0)
        return;

    static std::vector<uint8_t> allPlayerPositions;
    allPlayerPositions.resize(2 + (m_connections.size() * 16));
    
    uint16_t size = (uint16_t)m_connections.size();
    memcpy(&allPlayerPositions[0], &size, 2);

    // Maks je bil tukaj

    for (int i = 0; i < m_connections.size(); i++)
    {
        uint8_t* dest = &allPlayerPositions[2 + i * 16];
        memcpy(dest, &m_connections[i].Data, 16);

        //fmt::print("id: {}, pos: ({}, {}), rot: {}\n", m_connections[i]->Data.id, m_connections[i]->Data.position.x, m_connections[i]->Data.position.y, m_connections[i]->Data.rotation);
    }

    m_netServer.SendToAll(Net::Buf(allPlayerPositions), (uint16_t)NetMessage::AllPlayersPosition, Net::UnreliableDiscardOld);
}

void Server::UpdateBullets()
{
    for (auto& bullet : m_bullets)
    {
        bullet.pos += bullet.dir * 0.014f * NetCommon::BulletSpeed;
        bullet.timeToLive -= 0.014f;
    }

    if (m_bullets.size() > 0)
    {
        if (m_bullets.front().timeToLive < 0.0f)
        {
            m_bullets.pop_front();
        }
    }
}

void Server::UpdateCollisions()
{
    for (auto& conn : m_connections)
    {
        if (conn.Health == 0)
            continue;

        for (int i = 0; i < m_bullets.size(); i++)
        {
            NetShootT& bullet = m_bullets[i];

            if (conn.Data.id == bullet.ownerId)
                continue;

            float distSqr = (conn.Data.position.x - bullet.pos.x) * (conn.Data.position.x - bullet.pos.x)
                + (conn.Data.position.y - bullet.pos.y) * (conn.Data.position.y - bullet.pos.y);

            const float hitDistance = 1.5f;
            if (distSqr <= hitDistance)
            {
                PlayerHit(conn, bullet);

                m_bullets.erase(m_bullets.begin() + i);
                i--;
            }
        }
    }
}

void Server::UpdateReviveTime()
{
    for (auto& conn : m_connections)
    {
        if (conn.Health == 0)
        {
            conn.ReviveTime -= 0.014f;
            if (conn.ReviveTime < 0.0f)
            {
                conn.Health = 100;
                
                uint16_t id = (uint16_t)conn.Data.id;
                m_netServer.SendToAll(Net::Buf(id), (uint16_t)NetMessage::PlayerRevived, Net::Reliable);
            }
        }
    }
}

void Server::UpdatePowerUps()
{
    m_timeTillPowerUpSpawn -= 0.014f;

    if (m_timeTillPowerUpSpawn < 0.0f)
    {
        if (m_powerUpPositions.size() < m_maxNumOfPowerUps)
            SpawnPowerup();

        m_timeTillPowerUpSpawn = m_powerUpSpawnInterval;
    }
}

void Server::SpawnPowerup()
{
    glm::ivec2 pos;
    pos.x = rand() % 91 - 45;
    pos.y = rand() % 51 - 25;

    m_powerUpPositions.push_back(pos);

    m_netServer.SendToAll(Net::Buf(pos), (uint16_t)NetMessage::SpawnPowerUp, Net::Reliable);

    fmt::print("Spawned a new power up\n");
}

void Server::PlayerHit(Connection& hitConn, NetShootT& bullet)
{
    m_netServer.SendToAll(Net::Buf(bullet.bulletId), (uint16_t)NetMessage::DestroyBullet, Net::Unreliable);

    hitConn.Health -= 10;
    if (hitConn.Health > 100 || hitConn.Health == 0)
    {
        PlayerDied(hitConn, bullet);
    }

    m_netServer.SendTo(Net::Buf(hitConn.Health), (uint16_t)NetMessage::UpdateHealth, Net::Reliable, *hitConn.GetNetConn());
}

void Server::PlayerDied(Connection& diedConn, NetShootT& bullet)
{
    diedConn.Health = 0;
    diedConn.Data.position.y = -10000.0f;
    diedConn.ReviveTime = 3.0f;

    NetPlayerDiedT t;
    t.diedClientId = diedConn.Data.id;
    t.killedByClientId = bullet.ownerId;

    m_netServer.SendToAll(Net::Buf(t), (uint16_t)NetMessage::PlayerDied, Net::Reliable);
}

void Server::AddConnection(Net::Connection& netConn)
{
    Connection& conn = m_connections.emplace_back(&netConn);
    conn.Data.id = GetNewId();

    fmt::print("new client connected (id: {}) {}\n", conn.Data.id, conn.GetAddr().ToString());

    m_netServer.SendTo(Net::Buf("pozdravljen!"), (uint16_t)NetMessage::Hello, Net::Unreliable, netConn);
    m_netServer.SendTo(Net::Buf((uint16_t)conn.Data.id), (uint16_t)NetMessage::ClientId, Net::Reliable, netConn);

    for (int i = 0; i < m_connections.size() - 1; i++)
    {
        NetPlayerNameT t;
        t.id = (uint16_t)m_connections[i].Data.id;
        memcpy(t.name, m_connections[i].PlayerName.data(), m_connections[i].PlayerName.size() + 1);

        m_netServer.SendTo(Net::Buf(t), (uint16_t)NetMessage::PlayerName, Net::Reliable, netConn);
    }

    SendNumOfPlayers();

    for (const auto& bullet : m_bullets)
    {
        m_netServer.SendTo(Net::Buf(bullet), (uint16_t)NetMessage::Shoot, Net::Unreliable, netConn);
    }

    for (const auto& powerUpPos : m_powerUpPositions)
    {
        m_netServer.SendTo(Net::Buf(powerUpPos), (uint16_t)NetMessage::SpawnPowerUp, Net::Reliable, netConn);
    }
}

void Server::RemoveConnection(Net::Connection& netConn)
{	
    for (auto it = m_connections.begin(); it != m_connections.end(); it++)
    {
        if (it->GetAddr() == netConn.GetAddr())
        {
            fmt::print("client disconnected (id: {}) {}\n", it->Data.id, it->GetAddr().ToString());
            m_connections.erase(it);
            break;
        }
    }

    SendNumOfPlayers();
}

void Server::SendNumOfPlayers()
{
    uint16_t num = (uint16_t)m_connections.size();
    m_netServer.SendToAll(Net::Buf(num), (uint16_t)NetMessage::NumOfPlayers, Net::Reliable);
}

Connection* Server::FindConnectionFromAddr(Net::IPAddr addr)
{
    for (int i = 0; i < m_connections.size(); i++)
    {
        if (m_connections[i].GetAddr() == addr)
        {
            return &m_connections[i];
        }
    }

    fmt::print(fg(fmt::color::red), "Failed to find connection from addr: ");
    std::cout << addr << "\n";
    return nullptr;
}

void Server::DataReceive(void* data, size_t bytes, uint16_t msgType, Net::Connection& netConn)
{
    Connection* conn = FindConnectionFromAddr(netConn.GetAddr());
    if (conn == nullptr) return;

    switch ((NetMessage)msgType)
    {
    case NetMessage::Hello:
    {
        fmt::print("Received: {}\n", fmt::string_view((char*)data, bytes));
        break;
    }

    case NetMessage::PlayerPosition:
    {
        if (conn->Health == 0)
            break;

        NetPlayerPositionT* t = (NetPlayerPositionT*)data;
        conn->Data.position = t->pos;
        conn->Data.rotation = t->rot;

        //fmt::print("PlayerPosition {}: pos: {}, {}, rot: {}\n", conn.Data.id, conn.Data.position.x, conn.Data.position.y, conn.Data.rotation);
        break;
    }

    case NetMessage::PlayerName:
    {
        NetPlayerNameT* t = (NetPlayerNameT*)data;

        t->name[19] = '\0';
        conn->PlayerName = t->name;

        t->id = (uint16_t)conn->Data.id;

        m_netServer.SendToAll(Net::Buf(*t), (uint16_t)NetMessage::PlayerName, Net::Reliable);

        fmt::print("Received Player Name: {} (id: {})\n", conn->PlayerName, conn->Data.id);
        break;
    }

    case NetMessage::Shoot:
    {
        //fmt::print("Shoot");

        NetShootT* t = (NetShootT*)data;

        uint32_t newId = GetNewBulletId();
        t->bulletId = newId;

        m_netServer.SendToAll(Net::Buf(*t), (uint16_t)NetMessage::Shoot, Net::Reliable);

        NetShootT* bullet = &m_bullets.emplace_back();
        memcpy(bullet, t, sizeof(NetShootT));

        break;
    }

    case NetMessage::DestroyPowerUp:
    {
        glm::ivec2 pos = *(glm::ivec2*)data;
        m_powerUpPositions.erase(std::remove(m_powerUpPositions.begin(), m_powerUpPositions.end(), pos), m_powerUpPositions.end());
        
        m_netServer.SendToAll(Net::Buf(pos), (uint16_t)NetMessage::DestroyPowerUp, Net::Reliable);

        fmt::print("Picked up power up\n");
        break;
    }

    default:
        fmt::print(fg(fmt::color::red), "Received invalid NetMessage type: {}\n", (int)msgType);
    }
}
