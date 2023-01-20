#pragma once

#include <asio.hpp>
#include <memory>
#include <vector>

class Network
{
public:
	static void Connect();
	static void Disconnect();

private:
	static void HandleReceivedMessage(asio::error_code ec, size_t bytes);

private:
	static asio::io_context s_ioContext;
	static asio::ip::tcp::socket s_socket;

	static std::unique_ptr<std::thread> s_thrContext;

	static std::vector<char> s_receiveBuffer;
};
