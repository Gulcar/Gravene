#include "Input.h"

#include <GLFW/glfw3.h>
#include "Renderer.h"
#include <fmt/core.h>

void Input::Init()
{
	glfwSetKeyCallback(Renderer::GetWindow(), Input::GlfwKeyCallback);
	glfwSetMouseButtonCallback(Renderer::GetWindow(), Input::GlfwMouseCallback);
}

Input::KeyCallbackPtr Input::BindKeyDown(KeyCallback callback)
{
	KeyCallbackPtr cbptr = std::make_shared<KeyCallback>(callback);
	s_keydownCallbackList.insert(cbptr);
	return cbptr;
}

void Input::UnbindKeyDown(KeyCallbackPtr callback)
{
	auto it = s_keydownCallbackList.find(callback);
	s_keydownCallbackList.erase(it);
}

void Input::GlfwKeyCallback(struct GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (action == GLFW_PRESS)
	{
		for (auto& f : s_keydownCallbackList)
		{
			(*f)(key);
		}
	}
}

Input::MouseCallbackPtr Input::BindMouseDown(Input::MouseCallback callback)
{
	MouseCallbackPtr cbptr = std::make_shared<MouseCallback>(callback);
	s_mousedownCallbackList.insert(cbptr);
	return cbptr;
}

void Input::UnbindMouseDown(Input::MouseCallbackPtr callback)
{
	auto it = s_mousedownCallbackList.find(callback);
	s_mousedownCallbackList.erase(it);
}

void Input::GlfwMouseCallback(struct GLFWwindow* window, int button, int action, int mods)
{
	if (action == GLFW_PRESS)
	{
		for (auto& f : s_mousedownCallbackList)
		{
			(*f)(button);
		}
	}
}

std::unordered_set<Input::KeyCallbackPtr> Input::s_keydownCallbackList;
std::unordered_set<Input::MouseCallbackPtr> Input::s_mousedownCallbackList;