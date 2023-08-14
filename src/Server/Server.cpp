#include "Server.h"

#include "NetCommon.h"
#include <iostream>
#include <fmt/core.h>
#include <fmt/color.h>
using namespace std::placeholders;

void Server::Start(uint16_t port)
{
	m_netServer.Start(port);
	fmt::print("Listening on port {}\n", port);

	m_netServer.SetClientConnectedCallback([](Net::Connection& conn) {
		std::cout << "new client connected: " << conn.GetAddr() << "\n";
	});
	m_netServer.SetClientDisconnectedCallback([](Net::Connection& conn) {
		std::cout << "client disconnected: " << conn.GetAddr() << "\n";
	});

	m_netServer.SetDataReceiveCallback(std::bind(&Server::DataReceive, this, _1, _2, _3, _4));
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
		bullet.position += bullet.direction * 0.014f * NetCommon::BulletSpeed;
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
			Bullet& bullet = m_bullets[i];

			if (conn.Data.id == bullet.ownerId)
				continue;

			float distSqr = (conn.Data.position.x - bullet.position.x) * (conn.Data.position.x - bullet.position.x)
				+ (conn.Data.position.y - bullet.position.y) * (conn.Data.position.y - bullet.position.y);

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

void Server::PlayerHit(Connection& hitConn, Bullet& bullet)
{
	// TODO: mogoce reliable tukaj
	m_netServer.SendToAll(Net::Buf(bullet.bulletId), (uint16_t)NetMessage::DestroyBullet, Net::Unreliable);

	hitConn.Health -= 10;
	if (hitConn.Health > 100 || hitConn.Health == 0)
	{
		PlayerDied(hitConn, bullet);
	}

	m_netServer.SendTo(Net::Buf(hitConn.Health), (uint16_t)NetMessage::UpdateHealth, Net::Reliable, hitConn.GetNetConn());
}

void Server::PlayerDied(Connection& diedConn, Bullet& bullet)
{
	diedConn.Health = 0;
	diedConn.Data.position.y = -10000.0f;
	diedConn.ReviveTime = 3.0f;

	NetPlayerDiedT t;
	t.diedClientId = diedConn.Data.id;
	t.killedByClientId = bullet.ownerId;

	m_netServer.SendToAll(Net::Buf(t), (uint16_t)NetMessage::PlayerDied, Net::Reliable);
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

void Server::DataReceive(void* data, size_t bytes, uint16_t msgType, Net::Connection& conn)
{
	switch ((NetMessage)msgType)
	{
	}

	/*
	m_socket.async_receive_from(asio::buffer(m_receiveBuffer), m_receivingEndpoint, [&](asio::error_code ec, size_t bytes) {

		//fmt::print("received a packet!\n");
		if (ec == asio::error::connection_refused || ec == asio::error::eof || ec == asio::error::connection_reset)
		{
			for (int i = 0; i < m_connections.size(); i++)
			{
				if (m_connections[i].Endpoint == m_receivingEndpoint)
				{
					fmt::print("A client has disconnected (id: {})\n", m_connections[i].Data.id);
					m_connections.erase(m_connections.begin() + i);
					break;
				}
			}
			SendNumOfPlayers();
			AsyncReceive();
			return;
		}
		else if (ec) fmt::print(fg(fmt::color::red), "Error: {} ({})\n", ec.message(), ec.value());

		NetMessage type;
		memcpy(&type, &m_receiveBuffer[0], 2);

		switch (type)
		{
		case NetMessage::NewConnection:
		{
			Connection& conn = m_connections.emplace_back(m_receivingEndpoint, this);
			conn.Data.id = GetNewId();
			fmt::print("NewConnection (id: {}) {}\n", conn.Data.id, conn.Endpoint.address().to_string());

			NetMessage type = NetMessage::ApproveConnection;
			conn.Send(asio::buffer(&type, sizeof(type)));

			static std::string msg = "  pozdravljen!\0";
			type = NetMessage::Hello;
			memcpy(&msg[0], &type, 2);
			conn.Send(asio::buffer(msg));

			uint8_t idData[2 + 2];
			type = NetMessage::ClientId;
			memcpy(&idData[0], &type, 2);
			memcpy(&idData[2], &conn.Data.id, 2);
			conn.Send(asio::buffer(idData, sizeof(idData)));
			
			for (int i = 0; i < m_connections.size() - 1; i++)
			{
				std::string nameMsg = "    " + m_connections[i].PlayerName;
				type = NetMessage::PlayerName;
				memcpy(&nameMsg[0], &type, 2);
				uint16_t id = (uint16_t)m_connections[i].Data.id;
				memcpy(&nameMsg[2], &id, 2);

				conn.Send(asio::buffer(nameMsg.data(), nameMsg.length() + 1));
			}

			SendNumOfPlayers();

			for (const auto& bullet : m_bullets)
			{
				uint8_t data[2 + sizeof(Bullet)];
				type = NetMessage::Shoot;
				memcpy(&data[0], &type, 2);
				memcpy(&data[2], &bullet, sizeof(Bullet));
				conn.Send(asio::buffer(data, sizeof(data)));
			}

			for (const auto& powerUpPos : m_powerUpPositions)
			{
				uint8_t data[2 + sizeof(glm::ivec2)];
				type = NetMessage::SpawnPowerUp;
				memcpy(&data[0], &type, 2);
				memcpy(&data[2], &powerUpPos, sizeof(glm::ivec2));
				conn.Send(asio::buffer(data, sizeof(data)));
			}

			break;
		}

		case NetMessage::TerminateConnection:
		{
			for (int i = 0; i < m_connections.size(); i++)
			{
				if (m_connections[i].Endpoint == m_receivingEndpoint)
				{
					fmt::print("TerminateConnection (id: {})\n", m_connections[i].Data.id);
					m_connections.erase(m_connections.begin() + i);
					break;
				}
			}
			SendNumOfPlayers();
			break;
		}

		case NetMessage::Hello:
		{
			fmt::print("Received: {}\n", (char*)m_receiveBuffer.data() + 2);
			break;
		}

		case NetMessage::PlayerPosition:
		{
			Connection* conn = FindConnectionFromEndpoint(m_receivingEndpoint);
			if (conn == nullptr) break;

			if (conn->Health == 0)
				break;

			memcpy(&conn->Data.position, &m_receiveBuffer[2], 12);
			//fmt::print("PlayerPosition {}: pos: {}, {}, rot: {}\n", conn.Data.id, conn.Data.position.x, conn.Data.position.y, conn.Data.rotation);

			break;
		}

		case NetMessage::PlayerName:
		{
			Connection* conn = FindConnectionFromEndpoint(m_receivingEndpoint);
			if (conn == nullptr) break;
			
			size_t len = strlen((char*)&m_receiveBuffer[4]);
			conn->PlayerName.resize(len, ' ');
			memcpy(&conn->PlayerName[0], &m_receiveBuffer[4], len + 1);

			uint16_t id = (uint16_t)conn->Data.id;
			memcpy(&m_receiveBuffer[2], &id, 2);
			SendToAllConnections(asio::buffer(m_receiveBuffer.data(), 2 + 2 + len + 1));

			fmt::print("Received Player Name: {} (id: {})\n", conn->PlayerName, conn->Data.id);
			break;
		}

		case NetMessage::Shoot:
		{
			//fmt::print("Shoot");
			uint32_t newId = Bullet::GetNewId();
			memcpy(&m_receiveBuffer[2 + offsetof(Bullet, bulletId)], &newId, 4);
			SendToAllConnections(asio::buffer(m_receiveBuffer.data(), 2 + sizeof(Bullet)));
			Bullet* bullet = &m_bullets.emplace_back();
			memcpy(bullet, &m_receiveBuffer[2], sizeof(Bullet));
			break;
		}

		case NetMessage::DestroyPowerUp:
		{
			glm::ivec2 pos;
			memcpy(&pos, &m_receiveBuffer[2], sizeof(glm::ivec2));

			m_powerUpPositions.erase(std::remove(m_powerUpPositions.begin(), m_powerUpPositions.end(), pos), m_powerUpPositions.end());

			SendToAllConnections(asio::buffer(m_receiveBuffer, 2 + sizeof(glm::ivec2)));

			fmt::print("Picked up power up\n");

			break;
		}

		default:
			fmt::print(fg(fmt::color::red), "Received invalid NetMessage type: {}\n", (int)type);
		}

		AsyncReceive();
	});
	*/
}
