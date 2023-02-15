#include "Network.h"

#include <asio.hpp>
#include <fmt/core.h>
#include <fmt/color.h>
#include <thread>
#include "Server/NetCommon.h"
#include "Renderer.h"
#include "Text.h"

void Network::Connect(std::string_view ip)
{
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
		static bool printed = false;
		if (!printed) {
			fmt::print(fg(fmt::color::red), "Exception: {}\n", e.what());
			printed = true;
		}
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
		default:
			fmt::print(fg(fmt::color::red), "Received invalid net message type: {}\n", (int)type);
		}


		s_socket.async_receive(asio::buffer(s_receiveBuffer), HandleReceivedMessage);
	}
	else fmt::print(fg(fmt::color::red), "Error receiving message: {}\n", ec.message());
}

std::vector<RemoteClientData> Network::RemoteClients;
std::string Network::LocalPlayerName;
asio::io_context Network::s_ioContext;
asio::ip::udp::socket Network::s_socket(s_ioContext);
std::thread* Network::s_thrContext = nullptr;
std::array<uint8_t, 256> Network::s_receiveBuffer;
uint16_t Network::s_clientId;
bool Network::s_isConnected = false;
std::unordered_map<uint16_t, std::string> Network::s_allPlayerNames;
