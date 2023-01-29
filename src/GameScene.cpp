#include "GameScene.h"

#include "Renderer.h"
#include "Text.h"
#include <fmt/core.h>
#include "Input.h"
#include "Network.h"

void GameScene::Start()
{
	m_playerTexture = Renderer::LoadTexture("resources/player.png");
	m_starsTexture = Renderer::LoadTexture("resources/stars.jpg");
}

void GameScene::Update(float deltaTime)
{
	m_localPlayer.Update(deltaTime);
	Network::SendPlayerPosition(m_localPlayer.Position, m_localPlayer.Rotation);
}

void GameScene::Draw(float deltaTime)
{
	Renderer::NewFrame();

	const static glm::vec2 starsSize = glm::vec2(16.0f, 9.0f) * 2.0f;
	const static glm::vec3 starsColor = { 0.95f, 0.95f, 0.95f };
	Renderer::Draw(m_starsTexture, { 0.0f, 0.0f }, starsSize, 0.0f, starsColor);
	Renderer::Draw(m_starsTexture, { starsSize.x, 0.0f }, starsSize, 0.0f, starsColor);
	Renderer::Draw(m_starsTexture, { -starsSize.x, 0.0f }, starsSize, 0.0f, starsColor);
	Renderer::Draw(m_starsTexture, { 0.0f, starsSize.y }, starsSize, 0.0f, starsColor);
	Renderer::Draw(m_starsTexture, { 0.0f, -starsSize.y }, starsSize, 0.0f, starsColor);
	Renderer::Draw(m_starsTexture, { starsSize.x, starsSize.y }, starsSize, 0.0f, starsColor);
	Renderer::Draw(m_starsTexture, { -starsSize.x, starsSize.y }, starsSize, 0.0f, starsColor);
	Renderer::Draw(m_starsTexture, { starsSize.x, -starsSize.y }, starsSize, 0.0f, starsColor);
	Renderer::Draw(m_starsTexture, { -starsSize.x, -starsSize.y }, starsSize, 0.0f, starsColor);

	m_localPlayer.Draw();

	for (const auto& c : Network::RemoteClients)
	{
		if (c.id != Network::GetLocalClientId())
			Renderer::Draw(m_playerTexture, c.position, { 2.0f, 2.0f }, c.rotation, { 0.1f, 0.1f, 1.0f });
	}

	Text::WriteFps(deltaTime);
}

void GameScene::End()
{

}
