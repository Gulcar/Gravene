#pragma once

#include <cstdint>

enum class NetMessage : uint16_t
{
	// message string
	Hello,

	// from client: vec2 pos, float rot
	PlayerPosition,

	// from server: uint16_t id
	ClientId,

	// uint16_t size, { uint32_t id, vec2 pos, float rot } * size
	AllPlayersPosition,

	// uint16_t id, string name
	PlayerName,
};
