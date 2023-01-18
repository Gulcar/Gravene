#include "GameScene.h"

#include "Renderer.h"
#include "GLFW/glfw3.h"
#include "Text.h"
#include <fmt/core.h>
#include "Input.h"

void GameScene::Start()
{
	m_amongusImage = Renderer::LoadTexture("resources/amogus.png");
	m_playerTex = Renderer::LoadTexture("resources/player.png");
}

void GameScene::Update(float deltaTime)
{

}

void GameScene::Draw(float deltaTime)
{
	Renderer::NewFrame();

	glm::vec2 mousePos = Input::GetMouseWorldPos();
	float playerRot = std::atan2f(mousePos.y, mousePos.x);
	Renderer::Draw(m_playerTex, { 0,0 }, { 5,5 }, playerRot / 3.1415f * 180.0f, { 1.0f, 0.2f, 0.2f });

	static float updateFpsCounter = 0.15f;
	static std::string fpsString = fmt::format("{}fps", (int)(1.0f / deltaTime));
	updateFpsCounter -= deltaTime;
	if (updateFpsCounter < 0.0f)
	{
		fpsString = fmt::format("{}fps", (int)(1.0f / deltaTime));
		updateFpsCounter = 0.15f;
	}
	Text::Write(fpsString, { -Renderer::GetRightEdgeWorldPos() + 0.5f, 9.4f }, 0.8f);
}

void GameScene::End()
{

}
