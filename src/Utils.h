#pragma once

#ifdef WIN32
#include <Windows.h>
#endif

namespace Utils
{
	void EnableTerminalColors()
	{
#ifdef WIN32
		HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
		DWORD dwMode = 0;
		GetConsoleMode(hOut, &dwMode);
		dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
		SetConsoleMode(hOut, dwMode);
#endif
	}
}
