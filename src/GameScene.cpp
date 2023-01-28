#include "GameScene.h"

#include "Renderer.h"
#include "GLFW/glfw3.h"
#include "Text.h"
#include <fmt/core.h>
#include "Input.h"
#include "Network.h"

void GameScene::Start()
{
	m_playerTexture = Renderer::LoadTexture("resources/player.png");
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
	for (const auto& c : Network::RemoteClients)
	{
		if (c.id != Network::GetLocalClientId())
			Renderer::Draw(m_playerTexture, c.position, { 3.0f, 3.0f }, c.rotation, { 0.1f, 0.1f, 1.0f });
	}

	Text::WriteFps(deltaTime);
}

void GameScene::End()
{

}
