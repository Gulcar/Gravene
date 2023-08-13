#include "Network.h"

#include <asio.hpp>
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

	try
	{
		asio::ip::udp::endpoint endpoint(asio::ip::make_address_v4(ip), 7766);

		s_socket.async_connect(endpoint, [](const asio::error_code& ec) {

			if (!ec)
			{
				NetMessage msg = NetMessage::NewConnection;
				s_socket.send(asio::buffer(&msg, sizeof(msg)));

				s_socket.async_receive(asio::buffer(s_receiveBuffer), HandleReceivedMessage);

				Network::SendHello("pozdravljen to je client!");
			}
			else fmt::print(fg(fmt::color::red), "Error connecting to the server: {}\n", ec.message());
			});

		s_thrContext = new std::thread([&]() { s_ioContext.run(); });
	}
	catch (std::exception& e)
	{
		fmt::print(fg(fmt::color::red), "Exception: {}\n", e.what());
	}
}

void Network::Disconnect()
{
	if (s_thrContext != nullptr)
	{
		NetMessage msg = NetMessage::TerminateConnection;
		s_socket.send(asio::buffer(&msg, sizeof(msg)));

		s_ioContext.stop();
		s_thrContext->join();

		delete s_thrContext;
		s_thrContext = nullptr;
	}
}

void Network::SendHello(std::string msg)
{
	try
	{
		msg = "  " + msg + '\0';

		NetMessage type = NetMessage::Hello;

		memcpy(msg.data(), &type, 2);

		//s_socket.async_write_some(asio::buffer(msg), HandleSendMessage);
		s_socket.send(asio::buffer(msg));
	}
	catch (std::exception& e)
	{
		fmt::print(fg(fmt::color::red), "Exception: {}\n", e.what());
	}
}

void Network::SendPlayerPosition(glm::vec2 pos, float rot)
{
	if (s_isConnected == false)
		return;

	try
	{
		uint8_t data[2 + 8 + 4];

		NetMessage type = NetMessage::PlayerPosition;

		memcpy(&data[0], &type, 2);
		memcpy(&data[2], &pos, 8);
		memcpy(&data[10], &rot, 4);

		s_socket.send(asio::buffer(data, sizeof(data)));
	}
	catch (std::exception& e)
	{
		fmt::print(fg(fmt::color::red), "Exception: {}\n", e.what());
	}
}

void Network::SendPlayerName(std::string_view name)
{
	try
	{
		std::string msg = "    " + std::string(name) + '\0';

		NetMessage type = NetMessage::PlayerName;

		memcpy(&msg[0], &type, 2);
		//memcpy(&msg[2], &s_clientId, 2);

		s_socket.send(asio::buffer(msg));
	}
	catch (std::exception& e)
	{
		fmt::print(fg(fmt::color::red), "Exception: {}\n", e.what());
	}
}

void Network::SendShoot(glm::vec2 pos, glm::vec2 dir)
{
	uint8_t data[2 + sizeof(Bullet)];
	NetMessage type = NetMessage::Shoot;
	memcpy(&data[0], &type, 2);
	memcpy(&data[2], &pos, 8);
	memcpy(&data[10], &dir, 8);
	memcpy(&data[18], &s_clientId, 2);
	memcpy(&data[22], &s_bulletTimeToLive, 4);

	try
	{
		s_socket.send(asio::buffer(data, sizeof(data)));
	}
	catch (std::exception& e)
	{
		fmt::print(fg(fmt::color::red), "Exception: {}\n", e.what());
	}
}

void Network::SendPowerUpPickup(glm::ivec2 pos)
{
	uint8_t data[2 + sizeof(glm::ivec2)];
	NetMessage type = NetMessage::DestroyPowerUp;
	memcpy(&data[0], &type, 2);
	memcpy(&data[2], &pos, sizeof(glm::ivec2));

	try
	{
		s_socket.send(asio::buffer(data, sizeof(data)));
	}
	catch (std::exception& e)
	{
		fmt::print(fg(fmt::color::red), "Exception: {}\n", e.what());
	}
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

void Network::HandleReceivedMessage(asio::error_code ec, size_t bytes)
{
	if (!ec)
	{
		//fmt::print("Received: {}\n", (char*)s_receiveBuffer.data());
		NetMessage type;
		memcpy(&type, &s_receiveBuffer[0], 2);

		switch (type)
		{
		case NetMessage::ApproveConnection:
		{
			s_isConnected = true;
			fmt::print("connected to the server!\n");
			Network::SendPlayerName(LocalPlayerName);
			break;
		}
		case NetMessage::Hello:
		{
			fmt::print("Received: {}\n", (char*)(s_receiveBuffer.data() + 2));
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
			fmt::print(fg(fmt::color::red), "Received invalid net message type: {}\n", (int)type);
		}


		s_socket.async_receive(asio::buffer(s_receiveBuffer), HandleReceivedMessage);
	}
	else fmt::print(fg(fmt::color::red), "Error receiving message: {}\n", ec.message());
}

std::vector<RemoteClientData> Network::RemoteClients;
std::string Network::LocalPlayerName;
std::deque<Bullet> Network::Bullets;
std::vector<glm::ivec2> Network::PowerUpPositions;
asio::io_context Network::s_ioContext;
asio::ip::udp::socket Network::s_socket(s_ioContext);
std::thread* Network::s_thrContext = nullptr;
std::array<uint8_t, 256> Network::s_receiveBuffer;
uint16_t Network::s_clientId;
bool Network::s_isConnected = false;
uint32_t Network::s_localPlayerHealth = 100;
std::unordered_map<uint16_t, std::string> Network::s_allPlayerNames;
uint16_t Network::s_numOfPlayers = 0;
std::vector<uint16_t> Network::s_deadPlayers;
uint16_t Network::s_killedById;
uint16_t Network::s_killedId;
float Network::s_killTime = -10.0f;
float Network::s_hitTime = -10.0f;
