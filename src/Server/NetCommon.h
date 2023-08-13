#pragma once

#include <cstdint>
#include <glm/vec2.hpp>

enum class NetMessage : uint16_t
{
	// [deprecated]
	// from client: empty
	NewConnection,

	// [deprecated]
	// from server: empty
	ApproveConnection,

	// [deprecated]
	// empty
	TerminateConnection,

	// message string
	Hello,

	// from client: vec2 pos, float rot
	PlayerPosition,

	// from server: uint16_t id
	ClientId,

	// from server: uint16_t size, { uint32_t id, vec2 pos, float rot } * size
	AllPlayersPosition,

	// uint16_t id, string name
	PlayerName,

	// from server: uint16_t num
	NumOfPlayers,

	// vec2 pos, vec2 dir, uint16_t ownerId, float timeToLive, uint32_t bulletId
	Shoot,

	// from server: uint32_t bulletId
	DestroyBullet,

	// from server: int hp
	UpdateHealth,

	// from server: uint16_t diedClientId, uint16_t killedByClientId
	PlayerDied,

	// from server: uint16_t clientId
	PlayerRevived,

	// from server: int x, int y
	SpawnPowerUp,

	// int x, int y
	DestroyPowerUp,
};

struct NetPlayerPositionT
{
	glm::vec2 pos;
	float rot;
};

struct NetPlayerNameT
{
	uint16_t id;
	char name[20];
};

struct NetShootT
{
	glm::vec2 pos;
	glm::vec2 dir;
	uint16_t ownerId;
	float timeToLive;
	uint32_t bulletId;
};

struct NetDestroyPowerUpT
{
	int x;
	int y;
};

namespace NetCommon
{
	const float BulletSpeed = 12.0f;
}
