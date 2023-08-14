#pragma once

#include <vector>
#include "Connection.h"
#include "NetCommon.h"
#include <deque>
#include <GulcarNet/Server.h>
#include <memory>

class Server
{
public:
	void Start(uint16_t port);
	void Update();

private:
	void UpdateClientPositions();
	void UpdateBullets();
	void UpdateCollisions();
	void UpdateReviveTime();
	void UpdatePowerUps();

	void SpawnPowerup();

	void PlayerHit(Connection& hitConn, NetShootT& bullet);
	void PlayerDied(Connection& diedConn, NetShootT& bullet);

	void AddConnection(Net::Connection& conn);
	void RemoveConnection(Net::Connection& conn);

	void SendNumOfPlayers();

	Connection* FindConnectionFromAddr(Net::IPAddr addr);

	void DataReceive(void* data, size_t bytes, uint16_t msgType, Net::Connection& conn);

	static inline uint32_t GetNewBulletId()
	{
		static uint32_t id = 1;
		return id++;
	}

private:
	std::vector<Connection> m_connections;
	std::deque<NetShootT> m_bullets;

	std::vector<glm::ivec2> m_powerUpPositions;
	const int m_maxNumOfPowerUps = 10;
	float m_powerUpSpawnInterval = 25.0f;
	float m_timeTillPowerUpSpawn = m_powerUpSpawnInterval;

	Net::Server m_netServer;
};
