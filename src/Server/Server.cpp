#include "Server.h"

#include "NetCommon.h"
#include <iostream>
#include <fmt/core.h>
#include <fmt/color.h>

void Server::Start()
{
	AsyncAccept();
	m_ioContext.run();
}

void Server::Update()
{
	if (m_connections.size() == 0)
		return;

	static std::vector<uint8_t> allPlayerPositions;
	allPlayerPositions.resize(2 + 2 + (m_connections.size() * 16));

	NetMessage type = NetMessage::AllPlayersPosition;
	memcpy(&allPlayerPositions[0], &type, 2);
	uint16_t size = (uint16_t)m_connections.size();
	memcpy(&allPlayerPositions[2], &size, 2);

	// Maks je bil tukaj

	//fmt::print("sending all positions:\n");

	for (int i = 0; i < m_connections.size(); i++)
	{
		uint8_t* dest = &allPlayerPositions[2 + 2 + i * 16];
		memcpy(dest, &m_connections[i]->Data, 16);

		//fmt::print("id: {}, pos: ({}, {}), rot: {}\n", m_connections[i]->Data.id, m_connections[i]->Data.position.x, m_connections[i]->Data.position.y, m_connections[i]->Data.rotation);
	}

	asio::const_buffer allPlayerPositionsBuffer = asio::buffer(allPlayerPositions);


	for (int i = 0; i < m_connections.size(); i++)
	{
		if (m_connections[i]->IsConnected() == false)
		{
			m_connections.erase(m_connections.begin() + i);
			i--;
			continue;
		}

		m_connections[i]->Send(allPlayerPositionsBuffer);
	}
}

void Server::AsyncAccept()
{
	m_acceptor.async_accept([&](const asio::error_code& ec, asio::ip::tcp::socket peer)
	{
		if (!ec)
		{
			fmt::print("Received a connection: ");
			std::cout << peer.remote_endpoint() << "\n";

			m_connections.emplace_back(std::make_shared<Connection>(std::move(peer)));
		}
		else
		{
			fmt::print(fg(fmt::color::red), "Error when accepting a connection!\n{}\n", ec.message());
		}

		AsyncAccept();

	});
}
