#pragma once

#include <memory>
#include <asio.hpp>
#include <string>
#include <array>
#include <glm/vec2.hpp>

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
	Connection(asio::ip::udp::endpoint endpoint, class Server* server)
		: Endpoint(std::move(endpoint)), m_server(server)
	{
	}

	void Send(asio::const_buffer data);

private:
	class Server* m_server;

public:
	ClientData Data;
	std::string PlayerName = "";
	uint32_t Health = 100;
	float ReviveTime;

	asio::ip::udp::endpoint Endpoint;
};
