#include <fmt/core.h>
#include <GLFW/glfw3.h>
#include "Renderer.h"
#include "Text.h"
#include "Input.h"
#include "SceneManager.h"
#include "GameScene.h"
#include "MenuScene.h"

int main()
{
	fmt::print("pozdravljen svet\n");

	Renderer::Init();
	Text::Init("resources/bitmap_font.png", { 1024, 576 }, { 64, 96 });
	Input::Init();

	SceneManager::AssignScene<GameScene>("GameScene");
	SceneManager::AssignScene<MenuScene>("MenuScene");
	SceneManager::SwitchToScene("GameScene");

	while (!glfwWindowShouldClose(Renderer::GetWindow()))
	{
		if (glfwGetKey(Renderer::GetWindow(), GLFW_KEY_ESCAPE))
			glfwSetWindowShouldClose(Renderer::GetWindow(), true);

		SceneManager::Update();
	}

	Renderer::Destroy();
}
