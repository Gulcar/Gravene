#include "Network.h"

#include <asio.hpp>
#include <fmt/core.h>
#include <fmt/color.h>
#include <thread>

void Network::Connect()
{
	asio::ip::tcp::endpoint endpoint(asio::ip::make_address_v4("127.0.0.1"), 7766);

	s_socket.async_connect(endpoint, [](const asio::error_code& ec) {

		if (!ec)
		{
			fmt::print("Connected to the server!\n");
			s_socket.async_read_some(asio::buffer(s_receiveBuffer), HandleReceivedMessage);
		}
		else fmt::print(fg(fmt::color::red), "Error connecting to the server: {}\n", ec.message());
	});

	s_thrContext = std::make_unique<std::thread>([&]() { s_ioContext.run(); });
}

void Network::Disconnect()
{
	s_ioContext.stop();
	s_thrContext->join();
}

void Network::HandleReceivedMessage(asio::error_code ec, size_t bytes)
{
	if (!ec)
	{
		fmt::print("Received: {}\n", s_receiveBuffer.data());
		s_socket.async_read_some(asio::buffer(s_receiveBuffer), HandleReceivedMessage);
	}
	else fmt::print(fg(fmt::color::red), "Error receiving message: {}\n", ec.message());
}

asio::io_context Network::s_ioContext;
asio::ip::tcp::socket Network::s_socket(s_ioContext);
std::unique_ptr<std::thread> Network::s_thrContext;
std::vector<char> Network::s_receiveBuffer(256);
