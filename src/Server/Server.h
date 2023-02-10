#pragma once

#include <asio.hpp>
#include "Connection.h"

class Server
{
public:
	Server(uint16_t port)
		: m_endpoint(asio::ip::tcp::v4(), port),
		m_acceptor(m_ioContext, m_endpoint)
	{
	}

	void Start();

	void Update();

private:
	void AsyncAccept();

private:
	asio::io_context m_ioContext;
	asio::ip::tcp::endpoint m_endpoint;
	asio::ip::tcp::acceptor m_acceptor;

	std::vector<std::shared_ptr<Connection>> m_connections;
};
