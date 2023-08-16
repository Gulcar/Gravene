#include "GameScene.h"

#include "Renderer.h"
#include "Text.h"
#include <fmt/core.h>
#include "Input.h"
#include "Network.h"
#include <glm/gtc/random.hpp>
#include "Server/NetCommon.h"

void GameScene::Start()
{
	m_playerTexture = Renderer::LoadTexture("resources/player.png");
	m_starsTexture = Renderer::LoadTexture("resources/stars.jpg");
	m_pixelTexture = Renderer::LoadTexture("resources/white_pixel.png");
	m_bulletTexture = Renderer::LoadTexture("resources/bullet.png");
	m_powerUpTexture = Renderer::LoadTexture("resources/power_up.png");
	
	m_localPlayer.SetRandPos();
}

void GameScene::Update(float deltaTime)
{
	if (Network::IsAlive(Network::GetLocalClientId()))
	{
		m_localPlayer.Update(deltaTime);
		Network::SendPlayerPosition(m_localPlayer.Position, m_localPlayer.Rotation);
	}

	for (auto& bullet : Network::Bullets)
	{
		bullet.pos += bullet.dir * deltaTime * NetCommon::BulletSpeed;

		bullet.timeToLive -= deltaTime;
	}

	if (Network::Bullets.size() > 0)
	{
		if (Network::Bullets.front().timeToLive < 0.0f)
		{
			Network::Bullets.pop_front();
		}
	}

	for (int i = 0; i < Network::PowerUpPositions.size(); i++)
	{
		glm::vec2 p = Network::PowerUpPositions[i];
		float dist = glm::distance(p, m_localPlayer.Position);

		if (dist < 1.2f)
		{
			glm::vec3 color = { 153.0f / 255.0f, 229 / 255.0f, 80.0f / 255.0f };
			ParticleSystems.emplace_back(15, p, color, 8.0f, 0.3f, 1.0f);

			m_localPlayer.GivePowerup();

			Network::SendPowerUpPickup(Network::PowerUpPositions[i]);
			Network::PowerUpPositions.erase(Network::PowerUpPositions.begin() + i);
			i--;
		}
	}

	for (auto& particleSystem : ParticleSystems)
		particleSystem.Update(deltaTime);
	if (ParticleSystems.size() > 0 && ParticleSystems.front().IsDone())
		ParticleSystems.pop_front();

	if (Network::GetKillTime() + 0.25f > glfwGetTime())
	{
		glm::vec2 camPos = Renderer::GetCameraPos();
		camPos.x += glm::linearRand(-0.5f, 0.5f);
		camPos.y += glm::linearRand(-0.5f, 0.5f);
		Renderer::SetCameraPos(camPos);
	}
	else if (Network::GetHitTime() + 0.1f > glfwGetTime())
	{
		glm::vec2 camPos = Renderer::GetCameraPos();
		camPos.x += glm::linearRand(-0.1f, 0.1f);
		camPos.y += glm::linearRand(-0.1f, 0.1f);
		Renderer::SetCameraPos(camPos);
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

	if (Network::IsAlive(Network::GetLocalClientId()))
	{
		m_localPlayer.Draw();
		Text::Write(Network::LocalPlayerName, { m_localPlayer.Position.x, m_localPlayer.Position.y + 1.3f }, 0.75f, true, true);
		Renderer::Draw(m_pixelTexture, { m_localPlayer.Position.x, m_localPlayer.Position.y - 1.3f }, { 3.0f, 0.25f }, 0.0f, { 0.3f, 0.027f, 0.027f });
		float healthWidth = (float)Network::GetLocalPlayerHealth() / 100.0f * 3.0f;
		float healthOffset = (3.0f - healthWidth) / 2.0f;
		Renderer::Draw(m_pixelTexture, { m_localPlayer.Position.x - healthOffset, m_localPlayer.Position.y - 1.3f }, { healthWidth, 0.25f }, 0.0f, { 0.96f, 0.027f, 0.027f });
	}

	if (Network::PositionStates.size() > 0)
	for (const auto& c : Network::GetLatestPositions())
	{
		if (c.id != Network::GetLocalClientId() && Network::IsAlive(c.id))
		{
			auto interpolated = Network::GetInterpolatedMovement(c);
			Renderer::Draw(m_playerTexture, interpolated.position, { 2.0f, 2.0f }, interpolated.rotation, { 0.96f, 0.027f, 0.027f });
			Text::Write(Network::GetPlayerNameFromId(c.id), { interpolated.position.x, interpolated.position.y + 1.2f}, 0.75f, true, true);
		}
	}

	for (auto& powerUpPos : Network::PowerUpPositions)
		Renderer::Draw(m_powerUpTexture, powerUpPos, { 1.2f, 1.2f });

	for (auto& bullet : Network::Bullets)
		Renderer::Draw(m_bulletTexture, bullet.pos, { 1.5f, 1.5f }, 0.0f, { 250.0f / 255.0f, 230.0f / 255.0f, 0.0f });

	for (auto& particleSystem : ParticleSystems)
		particleSystem.Draw();

	Text::WriteFps(deltaTime);

	std::string dstr = fmt::format("D: {} kB/s ({} packets/s)",
		Network::GetStats().ReceivedBytesPerSecond() / 1000.0f,
		Network::GetStats().ReceivedPacketsPerSecond());
	std::string ustr = fmt::format("U: {} kB/s ({} packets/s)",
		Network::GetStats().SentBytesPerSecond() / 1000.0f,
		Network::GetStats().SentPacketsPerSecond());

	Text::Write(dstr, { -Renderer::GetRightEdgeWorldPos() + 0.5f, 8.6f }, 0.4f);
	Text::Write(ustr, { -Renderer::GetRightEdgeWorldPos() + 0.5f, 8.2f }, 0.4f);

	Text::WriteRightAligned(fmt::format("players:{}", Network::GetNumOfPlayers()), {Renderer::GetRightEdgeWorldPos(), 9.4f}, 0.8f);

	if (Network::IsAlive(Network::GetLocalClientId()) == false)
	{
		Text::Write("You died!", { 0, 0 }, 2.0f, true);
		Text::Write(fmt::format("Killed by {}", Network::GetKilledByName()), { 0, -2 }, 1.0f, true);
	}

	if (Network::GetKillTime() + 2.0f > glfwGetTime())
	{
		Text::Write(fmt::format("You killed {}!", Network::GetKillName()), { 0, -3 }, 0.9f, true);
	}
}

void GameScene::End()
{
	Network::Disconnect();
}
