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
	m_pixelTexture = Renderer::LoadTexture("resources/white_pixel.png");
	m_bulletTexture = Renderer::LoadTexture("resources/bullet.png");
}

void GameScene::Update(float deltaTime)
{
	m_localPlayer.Update(deltaTime);
	Network::SendPlayerPosition(m_localPlayer.Position, m_localPlayer.Rotation);

	for (auto& bullet : Network::Bullets)
	{
		const float bulletSpeed = 9.0f;
		bullet.position += bullet.direction * deltaTime * bulletSpeed;

		bullet.timeToLive -= deltaTime;
	}

	if (Network::Bullets.size() > 0)
	{
		if (Network::Bullets.front().timeToLive < 0.0f)
		{
			Network::Bullets.pop_front();
		}
	}
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
	Text::Write(Network::LocalPlayerName, {m_localPlayer.Position.x, m_localPlayer.Position.y + 1.2f}, 0.75f, true, true);

	for (const auto& c : Network::RemoteClients)
	{
		if (c.id != Network::GetLocalClientId())
		{
			Renderer::Draw(m_playerTexture, c.position, { 2.0f, 2.0f }, c.rotation, { 0.96f, 0.027f, 0.027f });
			Text::Write(Network::GetPlayerNameFromId(c.id), {c.position.x, c.position.y + 1.2f}, 0.75f, true, true);
		}
	}

	for (auto& bullet : Network::Bullets)
		Renderer::Draw(m_bulletTexture, bullet.position, { 1.5f, 1.5f }, 0.0f, { 250.0f / 255.0f, 230.0f / 255.0f, 0.0f });

	Text::WriteFps(deltaTime);
	Text::WriteRightAligned(fmt::format("players:{}", Network::GetNumOfPlayers()), {Renderer::GetRightEdgeWorldPos(), 9.4f}, 0.8f);
}

void GameScene::End()
{
	Network::Disconnect();
}
