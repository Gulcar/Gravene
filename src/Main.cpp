#include <fmt/core.h>
#include "Network.h"
#include "Utils.h"
#include <GLFW/glfw3.h>
#include "Renderer.h"
#include "Text.h"
#include "Input.h"
#include "SceneManager.h"
#include "GameScene.h"
#include "MenuScene.h"
#include "TestScene.h"

int main()
{
	fmt::print("pozdravljen svet\n");
	Utils::EnableTerminalColors();

	srand(time(nullptr));

	Renderer::Init();
	Text::Init("resources/bitmap_font.png", { 1024, 576 }, { 64, 96 });

	SceneManager::AssignScene<GameScene>("GameScene");
	SceneManager::AssignScene<MenuScene>("MenuScene");
	SceneManager::AssignScene<TestScene>("TestScene");
	SceneManager::SwitchToScene("MenuScene");

	//Network::Connect();
	//Network::SendHello("hello from main");

	while (!glfwWindowShouldClose(Renderer::GetWindow()))
	{
		if (glfwGetKey(Renderer::GetWindow(), GLFW_KEY_ESCAPE))
			glfwSetWindowShouldClose(Renderer::GetWindow(), true);

		Input::Poll();

		Network::Process();

		SceneManager::Update();
	}

	Network::Disconnect();
	Renderer::Destroy();
}
