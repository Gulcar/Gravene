#include "Input.h"

#include <GLFW/glfw3.h>
#include "Renderer.h"
#include <fmt/core.h>

void Input::Poll()
{
	for (int i = 32; i < 349; i++)
	{
		s_prevKeys[i] = s_keys[i];
		s_keys[i] = glfwGetKey(Renderer::GetWindow(), i);
	}

	for (int i = 0; i < 8; i++)
	{
		s_prevMouseButtons[i] = s_mouseButtons[i];
		s_mouseButtons[i] = glfwGetMouseButton(Renderer::GetWindow(), i);
	}
}

bool Input::GetKey(Input::Key key)
{
	return s_keys[key];
}

bool Input::GetKeyDown(Input::Key key)
{
	return (s_keys[key] && !s_prevKeys[key]);
}

bool Input::GetKeyUp(Input::Key key)
{
	return (!s_keys[key] && s_prevKeys[key]);
}

bool Input::GetMouseButton(Input::MouseButton button)
{
	return s_mouseButtons[button];
}

bool Input::GetMouseButtonDown(Input::MouseButton button)
{
	return (s_mouseButtons[button] && !s_prevMouseButtons[button]);
}

bool Input::GetMouseButtonUp(Input::MouseButton button)
{
	return (!s_mouseButtons[button] && s_prevMouseButtons[button]);
}

glm::vec2 Input::GetMousePos()
{
	double x, y;
	glfwGetCursorPos(Renderer::GetWindow(), &x, &y);

	return { x, y };
}

glm::vec2 Input::GetMouseWorldPos()
{
	glm::vec2 pos = GetMousePos();

	pos.x -= Renderer::GetWindowWidth() / 2.0f;
	pos.x /= (float)Renderer::GetWindowWidth() / 2.0f;
	pos.x *= ((float)Renderer::GetWindowWidth() / (float)Renderer::GetWindowHeight()) * 10.0f;

	pos.y -= Renderer::GetWindowHeight() / 2.0f;
	pos.y = -pos.y;
	pos.y = pos.y / (float)Renderer::GetWindowHeight() * 20.0f;

	return pos;
}

std::bitset<349> Input::s_keys(0);
std::bitset<349> Input::s_prevKeys(0);
std::bitset<8> Input::s_mouseButtons(0);
std::bitset<8> Input::s_prevMouseButtons(0);
