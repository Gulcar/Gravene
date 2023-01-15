#pragma once

#include <unordered_set>
#include <functional>
#include <memory>

class Input
{
public:
	using KeyCallback = std::function<void(int)>;
	using KeyCallbackPtr = std::shared_ptr<KeyCallback>;

	using MouseCallback = std::function<void(int)>;
	using MouseCallbackPtr = std::shared_ptr<MouseCallback>;

public:
	static void Init();

	// to bind: Input::KeyCallbackPtr callbackPtr = Input::BindKeyDown(std::bind(&Class::Function, this, std::placeholders::_1));
	static KeyCallbackPtr BindKeyDown(KeyCallback callback);
	// to unbind: Input::UnbindKeyDown(callbackPtr);
	static void UnbindKeyDown(KeyCallbackPtr callback);

	// to bind: Input::MouseCallbackPtr callbackPtr = Input::BindMouseDown(std::bind(&Class::Function, this, std::placeholders::_1));
	static MouseCallbackPtr BindMouseDown(MouseCallback callback);
	// to unbind: Input::UnbinMouseDown(callbackPtr);
	static void UnbindMouseDown(MouseCallbackPtr callback);

private:
	static void GlfwKeyCallback(struct GLFWwindow* window, int key, int scancode, int action, int mods);
	static std::unordered_set<KeyCallbackPtr> s_keydownCallbackList;

	static void GlfwMouseCallback(struct GLFWwindow* window, int button, int action, int mods);
	static std::unordered_set<MouseCallbackPtr> s_mousedownCallbackList;
};