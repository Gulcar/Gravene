#include "Network.h"

#include <asio.hpp>
#include <fmt/core.h>
#include <fmt/color.h>
#include <thread>
#include "Server/NetCommon.h"

void Network::Connect()
{
	asio::ip::tcp::endpoint endpoint(asio::ip::make_address_v4("127.0.0.1"), 7766);

	s_socket.async_connect(endpoint, [](const asio::error_code& ec) {

		if (!ec)
		{
			fmt::print("Connected to the server!\n");
			s_socket.async_read_some(asio::buffer(s_receiveBuffer), HandleReceivedMessage);

			Network::SendHello("pozdravljen to je client!");
			Network::SendPlayerPosition({ 1.0f, 3.0f }, 45.0f);
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

void Network::SendHello(std::string msg)
{
	try
	{
		msg = "  " + msg + '\0';

		NetMessage type = NetMessage::Hello;

		memcpy(msg.data(), &type, 2);

		//s_socket.async_write_some(asio::buffer(msg), HandleSendMessage);
		s_socket.write_some(asio::buffer(msg));
	}
	catch (std::exception& e)
	{
		fmt::print(fg(fmt::color::red), "Exception: {}\n", e.what());
	}
}

void Network::SendPlayerPosition(glm::vec2 pos, float rot)
{
	try
	{
		uint8_t data[2 + 8 + 4];

		NetMessage type = NetMessage::PlayerPosition;

		memcpy(&data[0], &type, 2);
		memcpy(&data[2], &pos, 8);
		memcpy(&data[10], &rot, 4);

		s_socket.write_some(asio::buffer(data, sizeof(data)));
	}
	catch (std::exception& e)
	{
		fmt::print(fg(fmt::color::red), "Exception: {}\n", e.what());
	}
}

void Network::HandleReceivedMessage(asio::error_code ec, size_t bytes)
{
	if (!ec)
	{
		fmt::print("Received: {}\n", (char*)s_receiveBuffer.data());
		s_socket.async_read_some(asio::buffer(s_receiveBuffer), HandleReceivedMessage);
	}
	else fmt::print(fg(fmt::color::red), "Error receiving message: {}\n", ec.message());
}

asio::io_context Network::s_ioContext;
asio::ip::tcp::socket Network::s_socket(s_ioContext);
std::unique_ptr<std::thread> Network::s_thrContext;
std::vector<uint8_t> Network::s_receiveBuffer(256);
