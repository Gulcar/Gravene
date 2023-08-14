#include <fmt/core.h>
#include <fmt/color.h>
#include <thread>
#include "Server.h"
#include "../Utils.h"

int main()
{
	const uint16_t PORT = 7766;
	
	fmt::print("Starting Server ...\n");
	Utils::EnableTerminalColors();
	srand(time(nullptr));

	Server server;
	server.Start(PORT);

	fmt::print("Server Started!\n");

	Utils::FpsLimiter fpsLimiter(14);

	while (true)
	{
		fpsLimiter.NewFrame();

		server.Update();
	}
}
