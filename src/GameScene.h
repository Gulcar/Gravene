#pragma once

#include "SceneManager.h"

class GameScene : public Scene
{
public:
	void Start() override;
	void Update(float deltaTime) override;
	void Draw(float deltaTime) override;
	void End() override;

private:
	uint32_t m_amongusImage;
	uint32_t m_playerTex;
};
