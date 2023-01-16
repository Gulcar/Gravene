#include "MenuScene.h"

#include "Renderer.h"
#include "Text.h"

void MenuScene::Start()
{

}

void MenuScene::Update(float deltaTime)
{

}

void MenuScene::Draw(float deltaTime)
{
	Renderer::NewFrame();

	Text::Write("Menu Scene", { 0,0 }, 1.0f, true);
}

void MenuScene::End()
{

}
