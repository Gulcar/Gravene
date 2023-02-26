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

	try
	{
		Server server(PORT);

		fmt::print("Server Started!\n");

		std::thread contextThr([&]() { server.Start(); });

		Utils::FpsLimiter fpsLimiter(14);

		while (true)
		{
			fpsLimiter.NewFrame();

			server.Update();
		}
	}
	catch (std::exception& e)
	{
		fmt::print(fg(fmt::color::red), "Error: Exception: {}\n", e.what());
	}
}
