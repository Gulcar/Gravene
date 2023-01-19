#pragma once

#include "SceneManager.h"
#include "Renderer.h"
#include "Text.h"
#include <GLFW/glfw3.h>
#include <cmath>
#include <fmt/core.h>

class TestScene : public Scene
{
public:
	void Start() override
	{
		m_amongusImage = Renderer::LoadTexture("resources/amogus.png");
	}

	void Update(float deltaTime) override
	{

	}

	void Draw(float deltaTime) override
	{
		Renderer::NewFrame();

		Renderer::Draw(m_amongusImage, { std::sinf(glfwGetTime()) * 5.0f, 0 }, { 10, 5 }, glfwGetTime() * 180.0f, { std::sinf(glfwGetTime()), std::cosf(glfwGetTime()), std::tanf(glfwGetTime()) });

		Text::Write("Pozdravljen Svet!!", { 0.0f, 0.0f }, 1.0f, true);

		Text::WriteFps(deltaTime);

		Text::Write(fmt::format("{}, {}", Input::GetMouseWorldPos().x, Input::GetMouseWorldPos().y), { 0.0f, 7.0f }, 0.75f, true);
	}
	
	void End() override
	{

	}
	
private:
	uint32_t m_amongusImage;
};