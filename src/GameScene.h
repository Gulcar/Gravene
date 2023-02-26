#pragma once

#include "SceneManager.h"
#include "LocalPlayer.h"
#include <cstdint>
#include <deque>
#include "ParticleSystem.h"

class GameScene : public Scene
{
public:
	void Start() override;
	void Update(float deltaTime) override;
	void Draw(float deltaTime) override;
	void End() override;

public:
	std::deque<ParticleSystem> ParticleSystems;

	inline LocalPlayer& GetLocalPlayer() { return m_localPlayer; }

private:
	LocalPlayer m_localPlayer;
	uint32_t m_playerTexture;
	uint32_t m_starsTexture;
	uint32_t m_pixelTexture;
	uint32_t m_bulletTexture;
	uint32_t m_powerUpTexture;
};
