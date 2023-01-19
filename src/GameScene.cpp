#include "GameScene.h"

#include "Renderer.h"
#include "GLFW/glfw3.h"
#include "Text.h"
#include <fmt/core.h>
#include "Input.h"

void GameScene::Start()
{

}

void GameScene::Update(float deltaTime)
{
	m_localPlayer.Update(deltaTime);
}

void GameScene::Draw(float deltaTime)
{
	Renderer::NewFrame();

	m_localPlayer.Draw();

	Text::WriteFps(deltaTime);
}

void GameScene::End()
{

}
