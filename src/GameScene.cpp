#include "GameScene.h"

#include "Renderer.h"
#include "GLFW/glfw3.h"
#include "Text.h"
#include <fmt/core.h>
#include "Input.h"

void GameScene::Start()
{
	m_amongusImage = Renderer::LoadTexture("resources/amogus.png");
}

void GameScene::Update(float deltaTime)
{

}

void GameScene::Draw(float deltaTime)
{
	Renderer::NewFrame();

	Renderer::Draw(m_amongusImage, { std::sinf(glfwGetTime()) * 5.0f, 0 }, { 10, 5 }, glfwGetTime() * 180.0f, { std::sinf(glfwGetTime()), std::cosf(glfwGetTime()), std::tanf(glfwGetTime()) });

	Text::Write("Pozdravljen Svet!!", { 0.0f, 0.0f }, 1.0f, true);

	static float updateFpsCounter = 0.15f;
	static std::string fpsString = fmt::format("{}fps", (int)(1.0f / deltaTime));
	updateFpsCounter -= deltaTime;
	if (updateFpsCounter < 0.0f)
	{
		fpsString = fmt::format("{}fps", (int)(1.0f / deltaTime));
		updateFpsCounter = 0.15f;
	}
	Text::Write(fpsString, { -Renderer::GetRightEdgeWorldPos() + 0.5f, 9.4f }, 0.8f);

	Text::Write(fmt::format("{}, {}", Input::GetMouseWorldPos().x, Input::GetMouseWorldPos().y), { 0.0f, 7.0f }, 0.75f, true);
}

void GameScene::End()
{

}
