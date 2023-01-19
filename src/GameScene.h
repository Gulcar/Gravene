#pragma once

#include "SceneManager.h"
#include "LocalPlayer.h"

class GameScene : public Scene
{
public:
	void Start() override;
	void Update(float deltaTime) override;
	void Draw(float deltaTime) override;
	void End() override;

private:
	LocalPlayer m_localPlayer;
};
