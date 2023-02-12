#include "Connection.h"

#include "NetCommon.h"
#include <fmt/core.h>
#include <fmt/color.h>
#include "Server.h"

void Connection::Send(asio::const_buffer data)
{
	try
	{
		m_server->GetSocket()->send_to(data, Endpoint);
	}
	catch (std::exception& e)
	{
		fmt::print(fg(fmt::color::red), "Exception: {}\n", e.what());
	}
}
