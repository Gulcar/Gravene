#include "Connection.h"

#include "NetCommon.h"
#include <fmt/core.h>
#include <fmt/color.h>

uint16_t GetNewId()
{
	static uint16_t id = 1;
	return id++;
}

Connection::Connection(asio::ip::tcp::socket&& socket)
	: m_socket(std::move(socket))
{
	Data.id = GetNewId();

	/*static std::string msg = "  pozdravljen!\0";
	NetMessage type = NetMessage::Hello;
	memcpy(&msg[0], &type, 2);
	Send(asio::buffer(msg));*/

	uint8_t idData[2 + 2];
	NetMessage type = NetMessage::ClientId;
	memcpy(&idData[0], &type, 2);
	memcpy(&idData[2], &Data.id, 2);
	Send(asio::buffer(idData, sizeof(idData)));

	m_socket.async_read_some(asio::buffer(m_receiveBuffer), std::bind(&Connection::HandleMessageReceived, this, std::placeholders::_1, std::placeholders::_2));
}

void Connection::Send(asio::const_buffer data)
{
	try
	{
		m_socket.write_some(data);
	}
	catch (std::exception& e)
	{
		fmt::print(fg(fmt::color::red), "Exception: {}\n", e.what());
	}
}

void Connection::HandleMessageReceived(const asio::error_code& ec, size_t bytes)
{
	if (!ec)
	{
		NetMessage type;
		memcpy(&type, m_receiveBuffer.data(), 2);

		switch (type)
		{
		case NetMessage::Hello:
		{
			fmt::print("Received: {}\n", (char*)(m_receiveBuffer.data() + 2));
			break;
		}
		case NetMessage::PlayerPosition:
		{
			memcpy(&Data.position, m_receiveBuffer.data() + 2, 8);
			memcpy(&Data.rotation, m_receiveBuffer.data() + 10, 4);
			//fmt::print("Received player {} position: {{ {}, {} }}, rot: {}\n", Data.id, Data.position.x, Data.position.y, Data.rotation);
			break;
		}
		case NetMessage::PlayerName:
		{
			PlayerName = "";

			int length = strlen((char*)&m_receiveBuffer[4]);

			PlayerName.resize(length, ' ');
			memcpy(PlayerName.data(), &m_receiveBuffer[4], length);

			fmt::print("Received PlayerName: {}\n", PlayerName);
			break;
		}
		default:
			fmt::print(fg(fmt::color::red), "Received invalid net message type: {}\n", (int)type);
		}

		m_socket.async_read_some(asio::buffer(m_receiveBuffer), std::bind(&Connection::HandleMessageReceived, this, std::placeholders::_1, std::placeholders::_2));
	}
	else if (ec == asio::error::eof || ec == asio::error::connection_reset)
	{
		fmt::print("A player has disconnected.\n");
		m_isConnected = false;
	}
	else
	{
		fmt::print(fg(fmt::color::red), "Error receiving message: {}\n", ec.message());
	}
}
