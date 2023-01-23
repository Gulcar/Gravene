#include "GameScene.h"

#include "Renderer.h"
#include "GLFW/glfw3.h"
#include "Text.h"
#include <fmt/core.h>
#include "Input.h"
#include "Network.h"

void GameScene::Start()
{

}

void GameScene::Update(float deltaTime)
{
	m_localPlayer.Update(deltaTime);
	Network::SendPlayerPosition(m_localPlayer.Position, m_localPlayer.Rotation);
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
