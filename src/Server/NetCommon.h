#pragma once

#include <cstdint>

enum class NetMessage : uint16_t
{
	// from client: empty
	NewConnection,

	// from server: empty
	ApproveConnection,

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
};
