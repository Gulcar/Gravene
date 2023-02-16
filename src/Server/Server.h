#pragma once

#include <asio.hpp>
#include <vector>
#include "Connection.h"

struct Bullet
{
	glm::vec2 position;
	glm::vec2 direction;
	uint16_t ownerId;
};

class Server
{
public:
	Server(uint16_t port)
		: m_endpoint(asio::ip::udp::v4(), port),
		m_socket(m_ioContext, m_endpoint)
	{
	}

	void Start();
	void Update();

	void SendToAllConnections(asio::const_buffer data);

	void SendNumOfPlayers();

	Connection* FindConnectionFromEndpoint(asio::ip::udp::endpoint endpoint);

	inline asio::ip::udp::socket* GetSocket() { return &m_socket; }

private:
	void AsyncReceive();

private:
	asio::io_context m_ioContext;
	asio::ip::udp::endpoint m_endpoint;
	asio::ip::udp::socket m_socket;

	std::vector<Connection> m_connections;
	std::vector<Bullet> m_bullets;
	
	std::array<uint8_t, 256> m_receiveBuffer;

	asio::ip::udp::endpoint m_receivingEndpoint;
};
