#pragma once

#include <memory>
#include <string>
#include <array>
#include <glm/vec2.hpp>
#include <GulcarNet/Server.h>

static uint16_t GetNewId()
{
    static uint16_t id = 1;
    return id++;
}

struct ClientData
{
    uint32_t id = 0;
    glm::vec2 position = { 0.0f, 0.0f };
    float rotation = 0.0f;
};

class Connection
{
public:
    Connection(Net::Connection* netConn)
        : m_netConn(netConn) {}

    inline Net::IPAddr GetAddr() const { return m_netConn->GetAddr(); }
    inline Net::Connection* GetNetConn() { return m_netConn; }

private:
    Net::Connection* m_netConn;

public:
    ClientData Data;
    std::string PlayerName = "";
    uint32_t Health = 100;
    float ReviveTime;
};
