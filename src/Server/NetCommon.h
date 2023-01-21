#pragma once

#include <cstdint>

enum class NetMessage : uint16_t
{
	// message string
	Hello,

	// vec2 pos, float rot
	PlayerPosition,
};
