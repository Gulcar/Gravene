#pragma once

#include <asio.hpp>
#include <array>
#include <cstdint>
#include <glm/vec2.hpp>
#include <string_view>

struct RemoteClientData
{
	uint32_t id;
	glm::vec2 position;
	float rotation;
};

class Network
{
public:
	static void Connect(std::string_view ip);
	static void Disconnect();

	static void SendHello(std::string msg);
	static void SendPlayerPosition(glm::vec2 pos, float rot);

	static inline uint16_t GetLocalClientId() { return s_clientId; }

private:
	static void HandleReceivedMessage(asio::error_code ec, size_t bytes);

public:
	static std::vector<RemoteClientData> RemoteClients;

private:
	static asio::io_context s_ioContext;
	static asio::ip::tcp::socket s_socket;

	static std::thread* s_thrContext;

	static std::array<uint8_t, 256> s_receiveBuffer;

	static uint16_t s_clientId;
};
