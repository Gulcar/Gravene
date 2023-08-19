#pragma once

#include "SceneManager.h"
#include "Renderer.h"
#include "Text.h"
#include "Input.h"
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

        auto pos = glm::vec2(std::sin(glfwGetTime()) * 5.0f, 0);
        auto color = glm::vec3(std::sin(glfwGetTime()), std::cos(glfwGetTime()), std::tan(glfwGetTime()));
		Renderer::Draw(m_amongusImage, pos, { 10, 5 }, glfwGetTime() * 180.0f, color);

		Text::Write("Pozdravljen Svet!!", { 0.0f, 0.0f }, 1.0f, true);

		Text::WriteFps(deltaTime);

        auto mouse = Input::GetMousePos();
		Text::Write(fmt::format("{}, {}", mouse.x, mouse.y), { 0.0f, 7.0f }, 0.75f, true);
	}
	
	void End() override
	{

	}
	
private:
	uint32_t m_amongusImage;
};
