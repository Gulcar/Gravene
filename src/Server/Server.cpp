#include "Server.h"

#include "NetCommon.h"
#include <iostream>
#include <fmt/core.h>
#include <fmt/color.h>

void Server::Start()
{
	AsyncReceive();
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

	for (int i = 0; i < m_connections.size(); i++)
	{
		uint8_t* dest = &allPlayerPositions[2 + 2 + i * 16];
		memcpy(dest, &m_connections[i].Data, 16);

		//fmt::print("id: {}, pos: ({}, {}), rot: {}\n", m_connections[i]->Data.id, m_connections[i]->Data.position.x, m_connections[i]->Data.position.y, m_connections[i]->Data.rotation);
	}

	asio::const_buffer allPlayerPositionsBuffer = asio::buffer(allPlayerPositions);

	SendToAllConnections(allPlayerPositionsBuffer);
}

void Server::SendToAllConnections(asio::const_buffer data)
{
	for (auto& conn : m_connections)
	{
		conn.Send(data);
	}
}

Connection* Server::FindConnectionFromEndpoint(asio::ip::udp::endpoint endpoint)
{
	for (int i = 0; i < m_connections.size(); i++)
	{
		if (m_connections[i].Endpoint == endpoint)
		{
			return &m_connections[i];
		}
	}

	fmt::print(fg(fmt::color::red), "Failed to find connection from endpoint: ");
	std::cout << endpoint << "\n";
	return nullptr;
}

void Server::AsyncReceive()
{
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
			fmt::print("NewConnection (id: {})\n", conn.Data.id);

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

		default:
			fmt::print(fg(fmt::color::red), "Received invalid NetMessage type: {}\n", (int)type);
		}

		AsyncReceive();
	});
}
