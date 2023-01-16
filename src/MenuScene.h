#pragma once

#include "SceneManager.h"

class MenuScene : public Scene
{
public:
	void Start() override;
	void Update(float deltaTime) override;
	void Draw(float deltaTime) override;
	void End() override;
};
