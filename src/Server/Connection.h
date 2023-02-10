#pragma once

#include <memory>
#include <asio.hpp>
#include <string>
#include <array>
#include <glm/vec2.hpp>

uint16_t GetNewId();

struct ClientData
{
	uint32_t id = 0;
	glm::vec2 position = { 0.0f, 0.0f };
	float rotation = 0.0f;
};

class Connection : public std::enable_shared_from_this<Connection>
{
public:
	Connection(asio::ip::tcp::socket&& socket);

	inline bool IsConnected() const { return m_isConnected; }

	void Send(asio::const_buffer data);

private:
	void HandleMessageReceived(const asio::error_code& ec, size_t bytes);

private:
	asio::ip::tcp::socket m_socket;
	bool m_isConnected = true;
	std::array<uint8_t, 256> m_receiveBuffer;

public:
	ClientData Data;
	std::string PlayerName = "";
};
