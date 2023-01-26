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

	// TODO
	AllPlayersPosition,
};
