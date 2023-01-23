#include <fmt/core.h>
#include <fmt/color.h>
#include <asio.hpp>
#include <iostream>
#include <vector>
#include "../Utils.h"
#include <functional>
#include "NetCommon.h"
#include <glm/vec2.hpp>

std::vector<uint8_t> g_receiveBuffer(256);

class Connection
{
public:
	Connection(asio::ip::tcp::socket&& socket)
		: m_socket(std::move(socket)) 
	{
		m_socket.async_read_some(asio::buffer(g_receiveBuffer), std::bind(&Connection::HandleMessageReceived, this, std::placeholders::_1, std::placeholders::_2));
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
		}
		else
		{
			fmt::print(fg(fmt::color::red), "Error receiving message: {}\n", ec.message());
		}
	}

private:
	asio::ip::tcp::socket m_socket;
};

asio::ip::tcp::acceptor* g_acceptor;
std::vector<Connection> g_connections;

void AcceptHandler(const asio::error_code& ec, asio::ip::tcp::socket peer)
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

		g_connections.emplace_back(std::move(peer));
	}
	else
	{
		fmt::print(fg(fmt::color::red), "Error when accepting a connection!\n{}\n", ec.message());
	}

	g_acceptor->async_accept(AcceptHandler);
}

int main()
{
	const int PORT = 7766;
	
	fmt::print("Starting Server ...\n");
	Utils::EnableTerminalColors();

	try
	{
		asio::io_context ioContext;

		asio::ip::tcp::endpoint endpoint(asio::ip::tcp::v4(), PORT);

		asio::ip::tcp::acceptor acceptor(ioContext, endpoint);
		g_acceptor = &acceptor;

		acceptor.async_accept(AcceptHandler);

		fmt::print("Server Started!\n");
		fmt::print("Listening on ");
		std::cout << acceptor.local_endpoint() << "\n";

		ioContext.run();
	}
	catch (std::exception& e)
	{
		fmt::print(fg(fmt::color::red), "Error: Exception: {}\n", e.what());
	}
}
