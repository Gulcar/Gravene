#pragma once

#include "SceneManager.h"
#include "LocalPlayer.h"
#include <cstdint>

class GameScene : public Scene
{
public:
	void Start() override;
	void Update(float deltaTime) override;
	void Draw(float deltaTime) override;
	void End() override;

private:
	LocalPlayer m_localPlayer;
	uint32_t m_playerTexture;
	uint32_t m_starsTexture;
	uint32_t m_pixelTexture;
};
