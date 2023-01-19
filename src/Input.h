#pragma once

#include <bitset>
#include <glm/vec2.hpp>
#include <GLFW/glfw3.h>

class Input
{
public:
	using Key = int;
	using MouseButton = int;

public:
	static void Poll();

	static bool GetKey(Key key);
	static bool GetKeyDown(Key key);
	static bool GetKeyUp(Key key);

	static bool GetMouseButton(MouseButton button);
	static bool GetMouseButtonDown(MouseButton button);
	static bool GetMouseButtonUp(MouseButton button);

	static glm::vec2 GetMousePos();
	static glm::vec2 GetMouseWorldPos();

private:
	static std::bitset<349> s_keys;
	static std::bitset<349> s_prevKeys;
	static std::bitset<8> s_mouseButtons;
	static std::bitset<8> s_prevMouseButtons;
};
