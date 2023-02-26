#pragma once

#include <asio.hpp>
#include <vector>
#include "Connection.h"
#include <deque>

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
	Server(uint16_t port)
		: m_endpoint(asio::ip::udp::v4(), port),
		m_socket(m_ioContext, m_endpoint)
	{
	}

	void Start();
	void Update();

	void UpdateClientPositions();
	void UpdateBullets();
	void UpdateCollisions();
	void UpdateReviveTime();
	void UpdatePowerUps();

	void SpawnPowerup();

	void PlayerHit(Connection& hitConn, Bullet& bullet);
	void PlayerDied(Connection& diedConn, Bullet& bullet);

	void PrintLocalIp();

	void SendToAllConnections(asio::const_buffer data);

	void SendNumOfPlayers();

	Connection* FindConnectionFromEndpoint(asio::ip::udp::endpoint endpoint);

	inline asio::ip::udp::socket* GetSocket() { return &m_socket; }

private:
	void AsyncReceive();

private:
	asio::io_context m_ioContext;
	asio::ip::udp::endpoint m_endpoint;
	asio::ip::udp::socket m_socket;

	std::vector<Connection> m_connections;
	std::deque<Bullet> m_bullets;

	std::vector<glm::ivec2> m_powerUpPositions;
	const int m_maxNumOfPowerUps = 10;
	float m_powerUpSpawnInterval = 25.0f;
	float m_timeTillPowerUpSpawn = m_powerUpSpawnInterval;
	
	std::array<uint8_t, 256> m_receiveBuffer;

	asio::ip::udp::endpoint m_receivingEndpoint;
};
