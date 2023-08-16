#include "Network.h"

#include <fmt/core.h>
#include <fmt/color.h>
#include "Server/NetCommon.h"
#include "Renderer.h"
#include "Text.h"
#include "SceneManager.h"
#include "Utils.h"
#include "GameScene.h"
#include <GLFW/glfw3.h>

void Network::Connect(std::string_view ip)
{
	s_netClient.SetConnectionStatusCallback([](Net::Client::Status status) {
		switch (status)
		{
		case Net::Client::Status::Connecting:
			fmt::print("connection status: Connecting\n"); break;
		case Net::Client::Status::Connected:
			fmt::print("connection status: Connected\n"); break;
		case Net::Client::Status::Disconnected:
			fmt::print(fg(fmt::color::red), "connection status: Disconnected\n"); break;
		case Net::Client::Status::FailedToConnect:
			SceneManager::SwitchToScene("MenuScene");
			fmt::print(fg(fmt::color::red), "connection status: FailedToConnect\n"); break;
		}
	});

	s_netClient.SetDataReceiveCallback(HandleReceivedMessage);

	s_netClient.Connect(Net::IPAddr(ip, 7766));

	Network::SendHello("pozdravljen to je client!");
	Network::SendPlayerName(LocalPlayerName);
}

void Network::Disconnect()
{
	s_netClient.Disconnect();
}

void Network::SendHello(const std::string& msg)
{
	s_netClient.Send(Net::Buf(msg), (uint16_t)NetMessage::Hello, Net::Reliable);
}

void Network::SendPlayerPosition(glm::vec2 pos, float rot)
{
	if (s_netClient.IsConnected() == false)
		return;
	if (Clock::now() - posSentTime < std::chrono::milliseconds(15))
		return;

	posSentTime = Clock::now();

	NetPlayerPositionT t;
	t.pos = pos;
	t.rot = rot;

	s_netClient.Send(Net::Buf(t), (uint16_t)NetMessage::PlayerPosition, Net::UnreliableDiscardOld);
}

void Network::SendPlayerName(std::string_view name)
{
	NetPlayerNameT t;
	memcpy(t.name, name.data(), name.length() + 1);

	s_netClient.Send(Net::Buf(t), (uint16_t)NetMessage::PlayerName, Net::Reliable);
}

void Network::SendShoot(glm::vec2 pos, glm::vec2 dir)
{
	NetShootT t;
	t.pos = pos;
	t.dir = dir;
	t.ownerId = s_clientId;
	t.timeToLive = s_bulletTimeToLive;

	s_netClient.Send(Net::Buf(t), (uint16_t)NetMessage::Shoot, Net::Reliable);
}

void Network::SendPowerUpPickup(glm::ivec2 pos)
{
	s_netClient.Send(Net::Buf(pos), (uint16_t)NetMessage::DestroyPowerUp, Net::Reliable);
}

const std::string& Network::GetPlayerNameFromId(uint16_t id)
{
	auto it = s_allPlayerNames.find(id);

	if (it != s_allPlayerNames.end())
	{
		return it->second;
	}

	static std::string empty = "";
	return empty;
}

RemoteClientData Network::GetInterpolatedMovement(const RemoteClientData& client)
{
	// https://www.gabrielgambetta.com/entity-interpolation.html
	// https://antriel.com/post/online-platformer-5/

	RemoteClientData interpolated = client;

	if (PositionStates.size() < 2)
		return interpolated;

	constexpr auto interpolationTime = std::chrono::milliseconds(60);
	auto time = Clock::now() - interpolationTime;

	int i;
	for (i = 0; i < PositionStates.size(); i++)
	{
		if (PositionStates[i].time > time)
			break;
	}

	if (i == 0)
		return interpolated;
	if (i == PositionStates.size())
		i--;

	const auto& stateBefore = PositionStates[i - 1];
	const auto& stateAfter = PositionStates[i];

	auto clientBefore = std::find_if(stateBefore.clients.begin(), stateBefore.clients.end(), [client](const RemoteClientData& d) {
		return d.id == client.id;
	});
	auto clientAfter = std::find_if(stateAfter.clients.begin(), stateAfter.clients.end(), [client](const RemoteClientData& d) {
		return d.id == client.id;
	});

	if (clientBefore == stateBefore.clients.end())
		return interpolated;
	if (clientAfter == stateAfter.clients.end())
		return interpolated;

	std::chrono::duration<float> timePassed = time - stateBefore.time;
	std::chrono::duration<float> timeBetween = stateAfter.time - stateBefore.time;

	float t = timePassed.count() / timeBetween.count();

	interpolated.position = Utils::Lerp(clientBefore->position, clientAfter->position, t);
	interpolated.rotation = Utils::LerpRotation(clientBefore->rotation, clientAfter->rotation, t);

	return interpolated;
}

bool Network::IsAlive(uint16_t id)
{
	for (int i = 0; i < s_deadPlayers.size(); i++)
	{
		if (s_deadPlayers[i] == id)
			return false;
	}

	return true;
}

void Network::Process()
{
	s_netClient.Process();

	if (PositionStates.size() > 0 &&
		Clock::now() - PositionStates.front().time > std::chrono::seconds(1))
	{
		PositionStates.pop_front();
	}
}

void Network::HandleReceivedMessage(void* data, size_t bytes, uint16_t msgType)
{
	switch ((NetMessage)msgType)
	{
	case NetMessage::Hello:
	{
		fmt::print("Received: {}\n", (char*)data);
		break;
	}
	case NetMessage::ClientId:
	{
		s_clientId = *(uint16_t*)data;
		fmt::print("ClientId: {}\n", s_clientId);
		break;
	}
	case NetMessage::AllPlayersPosition:
	{
		uint16_t size;
		memcpy(&size, data, 2);

		PositionState state;
		state.time = Clock::now();
		state.clients.resize(size);
		memcpy(&state.clients[0], (char*)data + 2, size * 16);

		PositionStates.push_back(std::move(state));

		//fmt::print("Received {} remote clients\n", size);
		//for (const auto& d : RemoteClients)
		//{
		//    fmt::print("id: {}, pos: ({}, {}) rot: {}\n", d.id, d.position.x, d.position.y, d.rotation);
		//}

		break;
	}
	case NetMessage::PlayerName:
	{
		NetPlayerNameT* t = (NetPlayerNameT*)data;
		fmt::print("Received PlayerName(id: {}): {}\n", t->id, t->name);
		s_allPlayerNames.insert({ t->id, std::string(t->name) });
		break;
	}
	case NetMessage::NumOfPlayers:
	{
		s_numOfPlayers = *(uint16_t*)data;
		break;
	}
	case NetMessage::Shoot:
	{
		NetShootT* bullet = &Bullets.emplace_back();
		memcpy(bullet, data, sizeof(NetShootT));
		break;
	}
	case NetMessage::DestroyBullet:
	{
		uint32_t bulletId = *(uint32_t*)data;

		for (int i = 0; i < Bullets.size(); i++)
		{
			if (Bullets[i].bulletId == bulletId)
			{
				GameScene* scene = (GameScene*)SceneManager::GetScene("GameScene");
				scene->ParticleSystems.emplace_back(20, Bullets[i].pos, glm::vec3(250.0f / 255.0f, 230.0f / 255.0f, 0.0f), 5.0f, 0.15f, 0.5f);
				Bullets.erase(Bullets.begin() + i);
				break;
			}
		}

		s_hitTime = glfwGetTime();
		break;
	}
	case NetMessage::UpdateHealth:
	{
		s_localPlayerHealth = *(uint32_t*)data;
		break;
	}
	case NetMessage::PlayerDied:
	{
		NetPlayerDiedT* t = (NetPlayerDiedT*)data;

		s_deadPlayers.push_back(t->diedClientId);

		if (s_clientId == t->diedClientId)
		{
			s_killedById = t->killedByClientId;
		}
		else if (s_clientId == t->killedByClientId)
		{
			s_killTime = glfwGetTime();
			s_killedId = t->diedClientId;
		}

		if (PositionStates.size() > 0)
		for (RemoteClientData& client : GetLatestPositions())
		{
			if (client.id == t->diedClientId)
			{
				glm::vec3 color = { 0.96f, 0.027f, 0.027f };
				if (client.id == s_clientId)
					color = { 0.067f, 0.341f, 0.941f };

				GameScene* scene = (GameScene*)SceneManager::GetScene("GameScene");
				scene->ParticleSystems.emplace_back(30, client.position, color, 8.0f, 0.5f, 1.0f);
				scene->ParticleSystems.emplace_back(30, client.position, color, 6.0f, 0.5f, 1.0f);
				break;
			}
		}
		break;
	}
	case NetMessage::PlayerRevived:
	{
		uint16_t id = *(uint16_t*)data;
		s_deadPlayers.erase(std::remove(s_deadPlayers.begin(), s_deadPlayers.end(), id), s_deadPlayers.end());

		if (id == s_clientId)
		{
			GameScene* scene = (GameScene*)SceneManager::GetScene("GameScene");
			scene->GetLocalPlayer().SetRandPos();
			s_localPlayerHealth = 100;
		}

		break;
	}
	case NetMessage::SpawnPowerUp:
	{
		glm::ivec2 pos = *(glm::ivec2*)data;
		PowerUpPositions.push_back(pos);
		break;
	}
	case NetMessage::DestroyPowerUp:
	{
		glm::ivec2 pos = *(glm::ivec2*)data;
		PowerUpPositions.erase(std::remove(PowerUpPositions.begin(), PowerUpPositions.end(), pos), PowerUpPositions.end());
		break;
	}
	default:
		fmt::print(fg(fmt::color::red), "Received invalid net message type: {}\n", msgType);
	}
}

std::string Network::LocalPlayerName;
std::deque<NetShootT> Network::Bullets;
std::vector<glm::ivec2> Network::PowerUpPositions;
uint16_t Network::s_clientId;
uint32_t Network::s_localPlayerHealth = 100;
std::unordered_map<uint16_t, std::string> Network::s_allPlayerNames;
uint16_t Network::s_numOfPlayers = 0;
std::vector<uint16_t> Network::s_deadPlayers;
uint16_t Network::s_killedById;
uint16_t Network::s_killedId;
float Network::s_killTime = -10.0f;
float Network::s_hitTime = -10.0f;
