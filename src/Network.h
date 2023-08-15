#pragma once

#include <array>
#include <cstdint>
#include <glm/vec2.hpp>
#include <string_view>
#include <functional>
#include <unordered_map>
#include <deque>
#include <GulcarNet/Client.h>
#include "Server/NetCommon.h"
#include <chrono>

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

	static void SendHello(const std::string& msg);
	static void SendPlayerPosition(glm::vec2 pos, float rot);
	static void SendPlayerName(std::string_view name);
	static void SendShoot(glm::vec2 pos, glm::vec2 dir);
	static void SendPowerUpPickup(glm::ivec2 pos);

	static inline uint16_t GetLocalClientId() { return s_clientId; }
	static inline uint32_t GetLocalPlayerHealth() { return s_localPlayerHealth; }

	static const std::string& GetPlayerNameFromId(uint16_t id);
	static inline uint16_t GetNumOfPlayers() { return s_numOfPlayers; }

	static inline std::vector<RemoteClientData>& GetLatestPositions() { return PositionStates.back().clients; }
	static RemoteClientData GetInterpolatedMovement(const RemoteClientData& client);

	static inline const std::string& GetKilledByName() { return GetPlayerNameFromId(s_killedById); }
	static inline float GetKillTime() { return s_killTime; }
	static inline const std::string& GetKillName() { return GetPlayerNameFromId(s_killedId); }
	static inline float GetHitTime() { return s_hitTime; }

	static bool IsAlive(uint16_t id);

	static void Process();

private:
	static void HandleReceivedMessage(void* data, size_t bytes, uint16_t msgType);

	using Clock = std::chrono::steady_clock;

public:
	struct PositionState
	{
		Clock::time_point time;
		std::vector<RemoteClientData> clients;
	};
	inline static std::deque<PositionState> PositionStates;

	static std::string LocalPlayerName;
	static std::deque<NetShootT> Bullets;
	static std::vector<glm::ivec2> PowerUpPositions;

private:
	static uint16_t s_clientId;
	static uint32_t s_localPlayerHealth;

	static std::unordered_map<uint16_t, std::string> s_allPlayerNames;
	static uint16_t s_numOfPlayers;

	static inline const float s_bulletTimeToLive = 5.0f;

	static std::vector<uint16_t> s_deadPlayers;
	static uint16_t s_killedById;
	static uint16_t s_killedId;
	static float s_killTime;
	static float s_hitTime;

	inline static Net::Client s_netClient;
};
