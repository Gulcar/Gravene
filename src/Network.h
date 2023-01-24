#pragma once

#include <asio.hpp>
#include <memory>
#include <array>
#include <cstdint>
#include <glm/vec2.hpp>

class Network
{
public:
	static void Connect();
	static void Disconnect();

	static void SendHello(std::string msg);
	static void SendPlayerPosition(glm::vec2 pos, float rot);

private:
	static void HandleReceivedMessage(asio::error_code ec, size_t bytes);

private:
	static asio::io_context s_ioContext;
	static asio::ip::tcp::socket s_socket;

	static std::unique_ptr<std::thread> s_thrContext;

	static std::array<uint8_t, 256> s_receiveBuffer;
};
