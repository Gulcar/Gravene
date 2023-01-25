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

std::array<uint8_t, 256> g_receiveBuffer;

class Connection : public std::enable_shared_from_this<Connection>
{
public:
	Connection(asio::ip::tcp::socket&& socket)
		: m_socket(std::move(socket)) 
	{
		m_socket.async_read_some(asio::buffer(g_receiveBuffer), std::bind(&Connection::HandleMessageReceived, this, std::placeholders::_1, std::placeholders::_2));
	}

	inline bool IsConnected() const
	{
		return m_isConnected;
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
				glm::vec2 pos;
				memcpy(&pos, g_receiveBuffer.data() + 2, 8);

				float rot;
				memcpy(&rot, g_receiveBuffer.data() + 10, 4);

				fmt::print("Received player position: {{ {}, {} }}, rot: {}\n", pos.x, pos.y, rot);

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
		for (int i = 0; i < m_connections.size(); i++)
		{
			if (m_connections[i]->IsConnected() == false)
			{
				m_connections.erase(m_connections.begin() + i);
				i--;
			}
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

				const static std::string message = "pozdravljen prek tcp!";
				peer.async_write_some(asio::buffer(message), [](const asio::error_code& ec, size_t bytesTransfered) {
					if (!ec) fmt::print("Transfered {} bytes\n", bytesTransfered);
					else fmt::print(fg(fmt::color::red), "async_write_some error: {}\n", ec.message());
					});

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
