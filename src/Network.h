#pragma once

#include <asio.hpp>
#include <array>
#include <cstdint>
#include <glm/vec2.hpp>
#include <string_view>
#include <functional>
#include <unordered_map>
#include <deque>

struct RemoteClientData
{
	uint32_t id;
	glm::vec2 position;
	float rotation;
};

struct Bullet
{
	glm::vec2 position;
	glm::vec2 direction;
	uint16_t ownerId;
	float timeToLive;
	uint32_t bulletId;
};

class Network
{
public:
	static void Connect(std::string_view ip);
	static void Disconnect();

	static void SendHello(std::string msg);
	static void SendPlayerPosition(glm::vec2 pos, float rot);
	static void SendPlayerName(std::string_view name);
	static void SendShoot(glm::vec2 pos, glm::vec2 dir);

	static inline uint16_t GetLocalClientId() { return s_clientId; }
	static inline uint32_t GetLocalPlayerHealth() { return s_localPlayerHealth; }

	static const std::string& GetPlayerNameFromId(uint16_t id);
	static inline uint16_t GetNumOfPlayers() { return s_numOfPlayers; }

	static inline const std::string& GetKilledByName() { return GetPlayerNameFromId(s_killedById); }
	static inline float GetKillTime() { return s_killTime; }
	static inline const std::string& GetKillName() { return GetPlayerNameFromId(s_killedId); }
	static inline float GetHitTime() { return s_hitTime; }

	static bool IsAlive(uint16_t id);

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
	static std::string LocalPlayerName;
	static std::deque<Bullet> Bullets;

private:
	static asio::io_context s_ioContext;
	static asio::ip::udp::socket s_socket;

	static std::thread* s_thrContext;

	static std::array<uint8_t, 256> s_receiveBuffer;

	static uint16_t s_clientId;
	static bool s_isConnected;
	static uint32_t s_localPlayerHealth;

	static std::unordered_map<uint16_t, std::string> s_allPlayerNames;
	static uint16_t s_numOfPlayers;

	static inline const float s_bulletTimeToLive = 5.0f;

	static std::vector<uint16_t> s_deadPlayers;
	static uint16_t s_killedById;
	static uint16_t s_killedId;
	static float s_killTime;
	static float s_hitTime;
};
