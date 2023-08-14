#pragma once

#include <vector>
#include "Connection.h"
#include <deque>
#include <GulcarNet/Server.h>

struct Bullet
{
	glm::vec2 position;
	glm::vec2 direction;
	uint16_t ownerId;
	float timeToLive;
	uint32_t bulletId;

	static inline uint32_t GetNewId()
	{
		static uint32_t id = 1;
		return id++;
	}
};

class Server
{
public:
	void Start(uint16_t port);
	void Update();

	void UpdateClientPositions();
	void UpdateBullets();
	void UpdateCollisions();
	void UpdateReviveTime();
	void UpdatePowerUps();

	void SpawnPowerup();

	void PlayerHit(Connection& hitConn, Bullet& bullet);
	void PlayerDied(Connection& diedConn, Bullet& bullet);

	void SendNumOfPlayers();

	Connection* FindConnectionFromAddr(Net::IPAddr addr);

private:
	void DataReceive(void* data, size_t bytes, uint16_t msgType, Net::Connection& conn);

private:
	std::vector<Connection> m_connections;
	std::deque<Bullet> m_bullets;

	std::vector<glm::ivec2> m_powerUpPositions;
	const int m_maxNumOfPowerUps = 10;
	float m_powerUpSpawnInterval = 25.0f;
	float m_timeTillPowerUpSpawn = m_powerUpSpawnInterval;

	Net::Server m_netServer;
};
