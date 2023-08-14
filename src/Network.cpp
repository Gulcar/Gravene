#include "Network.h"

#include <fmt/core.h>
#include <fmt/color.h>
#include <thread>
#include "Server/NetCommon.h"
#include "Renderer.h"
#include "Text.h"
#include "SceneManager.h"
#include "GameScene.h"
#include <GLFW/glfw3.h>

void Network::Connect(std::string_view ip)
{
	s_netClient.Connect(Net::IPAddr(ip, 7766));

	Network::SendHello("pozdravljen to je client!");
	Network::SendPlayerName(LocalPlayerName);

	s_netClient.SetConnectionStatusCallback([](Net::Client::Status status) {
		switch (status)
		{
		case Net::Client::Status::Connecting:
			fmt::print("connection status: Connecting\n"); break;
		case Net::Client::Status::Connected:
			fmt::print("connection status: Connected\n"); break;
		case Net::Client::Status::Disconnected:
			fmt::print("connection status: Disconnected\n"); break;
		case Net::Client::Status::FailedToConnect:
			fmt::print("connection status: FailedToConnect\n"); break;
		}
	});

	s_netClient.SetDataReceiveCallback(HandleReceivedMessage);

	// TODO: preveri NetMessage::NewConnection
}

void Network::Disconnect()
{
	s_netClient.Disconnect();
	// TODO: preveri NetMessage::TerminateConnection
}

void Network::SendHello(const std::string& msg)
{
	s_netClient.Send(Net::Buf(msg), (uint16_t)NetMessage::Hello, Net::Reliable);
}

void Network::SendPlayerPosition(glm::vec2 pos, float rot)
{
	if (s_netClient.IsConnected() == false)
		return;

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
	NetDestroyPowerUpT t;
	t.x = pos.x;
	t.y = pos.y;

	s_netClient.Send(Net::Buf(t), (uint16_t)NetMessage::DestroyPowerUp, Net::Reliable);
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
}

void Network::HandleReceivedMessage(void* data, size_t bytes, uint16_t msgType)
{
	/* TODO: vse to
	switch ((NetMessage)msgType)
	{
	case NetMessage::Hello:
	{
		fmt::print("Received: {}\n", (char*)data);
		break;
	}
	case NetMessage::ClientId:
	{
		memcpy(&s_clientId, s_receiveBuffer.data() + 2, 2);
		fmt::print("ClientId: {}\n", s_clientId);
		break;
	}
	case NetMessage::AllPlayersPosition:
	{
		uint16_t size;
		memcpy(&size, &s_receiveBuffer[2], 2);

		RemoteClients.resize(size);
		memcpy(&RemoteClients[0], &s_receiveBuffer[4], size * 16);

		//fmt::print("Received {} remote clients\n", size);
		//for (const auto& d : RemoteClients)
		//{
		//    fmt::print("id: {}, pos: ({}, {}) rot: {}\n", d.id, d.position.x, d.position.y, d.rotation);
		//}

		break;
	}
	case NetMessage::PlayerName:
	{
		uint16_t id;
		std::string name;

		memcpy(&id, &s_receiveBuffer[2], 2);

		size_t len = strlen((char*)&s_receiveBuffer[4]);
		name.resize(len);
		memcpy(&name[0], &s_receiveBuffer[4], len + 1);

		fmt::print("Received PlayerName(id: {}): {}\n", id, name);
		s_allPlayerNames.insert({ id, name });
		break;
	}
	case NetMessage::NumOfPlayers:
	{
		memcpy(&s_numOfPlayers, &s_receiveBuffer[2], 2);
		break;
	}
	case NetMessage::Shoot:
	{
		Bullet* bullet = &Bullets.emplace_back();
		memcpy(bullet, &s_receiveBuffer[2], sizeof(Bullet));
		break;
	}
	case NetMessage::DestroyBullet:
	{
		uint32_t bulletId;
		memcpy(&bulletId, &s_receiveBuffer[2], 4);

		for (int i = 0; i < Bullets.size(); i++)
		{
			if (Bullets[i].bulletId == bulletId)
			{
				GameScene* scene = (GameScene*)SceneManager::GetScene("GameScene");
				scene->ParticleSystems.emplace_back(20, Bullets[i].position, glm::vec3(250.0f / 255.0f, 230.0f / 255.0f, 0.0f), 5.0f, 0.15f, 0.5f);
				Bullets.erase(Bullets.begin() + i);
				break;
			}
		}

		s_hitTime = glfwGetTime();
		break;
	}
	case NetMessage::UpdateHealth:
	{
		memcpy(&s_localPlayerHealth, &s_receiveBuffer[2], 4);
		break;
	}
	case NetMessage::PlayerDied:
	{
		uint16_t id;
		memcpy(&id, &s_receiveBuffer[2], 2);
		uint16_t killerId;
		memcpy(&killerId, &s_receiveBuffer[4], 2);

		s_deadPlayers.push_back(id);

		if (s_clientId == id)
		{
			s_killedById = killerId;
		}
		else if (s_clientId == killerId)
		{
			s_killTime = glfwGetTime();
			s_killedId = id;
		}

		for (RemoteClientData& client : RemoteClients)
		{
			if (client.id == id)
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
		uint16_t id;
		memcpy(&id, &s_receiveBuffer[2], 2);
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
		glm::ivec2 pos;
		memcpy(&pos, &s_receiveBuffer[2], sizeof(pos));
		PowerUpPositions.push_back(pos);
		break;
	}
	case NetMessage::DestroyPowerUp:
	{
		glm::ivec2 pos;
		memcpy(&pos, &s_receiveBuffer[2], sizeof(pos));
		PowerUpPositions.erase(std::remove(PowerUpPositions.begin(), PowerUpPositions.end(), pos), PowerUpPositions.end());
		break;
	}
	default:
		fmt::print(fg(fmt::color::red), "Received invalid net message type: {}\n", msgType);
	}
	*/
}

std::vector<RemoteClientData> Network::RemoteClients;
std::string Network::LocalPlayerName;
std::deque<Bullet> Network::Bullets;
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
