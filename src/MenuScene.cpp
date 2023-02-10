#include "MenuScene.h"

#include "Renderer.h"
#include "Text.h"
#include "Network.h"

void MenuScene::Start()
{
	m_starsTexture = Renderer::LoadTexture("resources/stars.jpg");

	m_playBtn.SetPosAndSize({ 0.0f, -4.5f }, { 6.0f, 1.4f });
	m_playBtn.SetText("Play");

	m_playerNameInput.SetPosAndSize({ 0.0f, 1.5f }, { 10.0f, 1.4f });

	m_serverIpInput.SetPosAndSize( { 0.0f, -1.6}, {10.0f, 1.4f} );
	m_serverIpInput.GetInput() = "127.0.0.1";
}

void MenuScene::Update(float deltaTime)
{
	m_playBtn.Update();
	m_playerNameInput.Update();
	m_serverIpInput.Update();

	if (m_playBtn.Clicked())
	{
		Network::Connect(m_serverIpInput.GetInput());
		Network::SendPlayerName(m_playerNameInput.GetInput());
		SceneManager::SwitchToScene("GameScene");
	}
}

void MenuScene::Draw(float deltaTime)
{
	Renderer::NewFrame();

	Renderer::Draw(m_starsTexture, { 0.0f, 0.0f }, glm::vec2(16.0f, 9.0f) * 3.0f, 0.0f, glm::vec3(0.95f));

	Text::Write("GRAVENE", { 0.0f, 6.0f }, 2.5f, true);

	m_playerNameInput.Draw();
	Text::Write("Player Name:", { 0.0f, 2.6f }, 1.0f, true);

	m_serverIpInput.Draw();
	Text::Write("Server IP:", { 0.0f, -0.5f }, 1.0f, true);

	m_playBtn.Draw();
}

void MenuScene::End()
{

}
