#include <fmt/core.h>
#include <fmt/color.h>
#include <fmt/chrono.h>
#include <asio.hpp>
#include <iostream>
#include <vector>
#include "../Utils.h"
#include <functional>
#include "NetCommon.h"
#include <glm/vec2.hpp>
#include <array>
#include <memory>

uint16_t GetNewId()
{
	static uint16_t id = 1;
	return id++;
}

struct ClientData
{
	uint32_t id;
	glm::vec2 position = { 0.0f, 0.0f };
	float rotation = 0.0f;
};

std::array<uint8_t, 256> g_receiveBuffer;

class Connection : public std::enable_shared_from_this<Connection>
{
public:
	Connection(asio::ip::tcp::socket&& socket)
		: m_socket(std::move(socket)) 
	{
		Data.id = GetNewId();

		static std::string msg = "  pozdravljen!\0";
		NetMessage type = NetMessage::Hello;
		memcpy(&msg[0], &type, 2);
		Send(asio::buffer(msg));

		uint8_t idData[2 + 2];
		type = NetMessage::ClientId;
		memcpy(&idData[0], &type, 2);
		memcpy(&idData[2], &Data.id, 2);
		Send(asio::buffer(idData, sizeof(idData)));

		m_socket.async_read_some(asio::buffer(g_receiveBuffer), std::bind(&Connection::HandleMessageReceived, this, std::placeholders::_1, std::placeholders::_2));
	}

	inline bool IsConnected() const
	{
		return m_isConnected;
	}

	void Send(asio::const_buffer data)
	{
		try
		{
			m_socket.write_some(data);
		}
		catch (std::exception& e)
		{
			fmt::print(fg(fmt::color::red), "Exception: {}\n", e.what());
		}
	}

private:
	void HandleMessageReceived(const asio::error_code& ec, size_t bytes)
	{
		if (!ec)
		{
			NetMessage type;
			memcpy(&type, g_receiveBuffer.data(), 2);

			switch (type)
			{
			case NetMessage::Hello:
			{
				fmt::print("Received: {}\n", (char*)(g_receiveBuffer.data() + 2));
				break;
			}
			case NetMessage::PlayerPosition:
			{
				memcpy(&Data.position, g_receiveBuffer.data() + 2, 8);
				memcpy(&Data.rotation, g_receiveBuffer.data() + 10, 4);
				//fmt::print("Received player position: {{ {}, {} }}, rot: {}\n", Data.position.x, Data.position.y, Data.rotation);
				break;
			}
			default:
				fmt::print(fg(fmt::color::red), "Received invalid net message type: {}\n", (int)type);
			}
			
			m_socket.async_read_some(asio::buffer(g_receiveBuffer), std::bind(&Connection::HandleMessageReceived, this, std::placeholders::_1, std::placeholders::_2));
		}
		else if (ec == asio::error::eof || ec == asio::error::connection_reset)
		{
			fmt::print("A player has disconnected.\n");
			m_isConnected = false;
		}
		else
		{
			fmt::print(fg(fmt::color::red), "Error receiving message: {}\n", ec.message());
		}
	}

private:
	asio::ip::tcp::socket m_socket;
	bool m_isConnected = true;

public:
	ClientData Data;
};

class Server
{
public:
	Server(uint16_t port)
		: m_endpoint(asio::ip::tcp::v4(), port),
		  m_acceptor(m_ioContext, m_endpoint)
	{

	}

	void Start()
	{
		AsyncAccept();
		m_ioContext.run();
	}

	void Update()
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
			memcpy(dest, &m_connections[i]->Data, 16);
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

private:
	void AsyncAccept()
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

private:
	asio::io_context m_ioContext;
	asio::ip::tcp::endpoint m_endpoint;
	asio::ip::tcp::acceptor m_acceptor;

	std::vector<std::shared_ptr<Connection>> m_connections;
};

int main()
{
	const uint16_t PORT = 7766;
	
	fmt::print("Starting Server ...\n");
	Utils::EnableTerminalColors();

	try
	{
		Server server(PORT);

		fmt::print("Server Started!\n");
		fmt::print("Listening on port {}\n", PORT);

		std::thread contextThr([&]() { server.Start(); });

		Utils::FpsLimiter fpsLimiter(14);

		while (true)
		{
			fpsLimiter.NewFrame();

			server.Update();
		}
	}
	catch (std::exception& e)
	{
		fmt::print(fg(fmt::color::red), "Error: Exception: {}\n", e.what());
	}
}
