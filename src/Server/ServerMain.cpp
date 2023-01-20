#include <fmt/core.h>
#include <fmt/color.h>
#include <asio.hpp>
#include <iostream>
#include <vector>
#include "../Utils.h"

asio::ip::tcp::acceptor* g_acceptor;
std::vector<asio::ip::tcp::socket> g_sockets;

void AcceptHandler(const asio::error_code& ec, asio::ip::tcp::socket peer)
{
	if (!ec)
	{
		fmt::print("Received a connection: ");
		std::cout << peer.remote_endpoint() << "\n";

		const static std::string message = "pozdravljen prek tcp!";
		peer.async_write_some(asio::buffer(message), [](const asio::error_code& ec, size_t bytesTransfered) {
			if (!ec) fmt::print("Transfered {} bytes\n", bytesTransfered);
			else fmt::print(fg(fmt::color::red), "async_write_some error: {}\n", ec.message());
		});

		g_sockets.emplace_back(std::move(peer));
	}
	else
	{
		fmt::print(fg(fmt::color::red), "Error when accepting a connection!\n{}\n", ec.message());
	}

	g_acceptor->async_accept(AcceptHandler);
}

int main()
{
	const int PORT = 7766;
	
	fmt::print("Starting Server ...\n");
	Utils::EnableTerminalColors();

	try
	{
		asio::io_context ioContext;

		asio::ip::tcp::endpoint endpoint(asio::ip::tcp::v4(), PORT);

		asio::ip::tcp::acceptor acceptor(ioContext, endpoint);
		g_acceptor = &acceptor;

		acceptor.async_accept(AcceptHandler);

		fmt::print("Server Started!\n");
		fmt::print("Listening on ");
		std::cout << acceptor.local_endpoint() << "\n";

		ioContext.run();
	}
	catch (std::exception& e)
	{
		fmt::print(fg(fmt::color::red), "Error: Exception: {}\n", e.what());
	}
}
