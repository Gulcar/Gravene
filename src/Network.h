#pragma once

#include <asio.hpp>
#include <array>
#include <cstdint>
#include <glm/vec2.hpp>
#include <string_view>
#include <functional>
#include <unordered_map>

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
	static void SendPlayerName(std::string_view name);

	static inline uint16_t GetLocalClientId() { return s_clientId; }

	static inline const std::string& GetLocalPlayerName() { return GetPlayerNameFromId(s_clientId); }
	static const std::string& GetPlayerNameFromId(uint16_t id);

private:
	static void HandleReceivedMessage(asio::error_code ec, size_t bytes);

	template<typename HandlerType>
	static void TryAgainLater(HandlerType handler)
	{
		static asio::steady_timer t(s_ioContext, asio::chrono::milliseconds(50));
		t.async_wait(handler);
	}

public:
	static std::vector<RemoteClientData> RemoteClients;

private:
	static asio::io_context s_ioContext;
	static asio::ip::udp::socket s_socket;

	static std::thread* s_thrContext;

	static std::array<uint8_t, 256> s_receiveBuffer;

	static uint16_t s_clientId;

	static bool s_isConnected;

	static std::unordered_map<uint16_t, std::string> s_allPlayerNames;
};
