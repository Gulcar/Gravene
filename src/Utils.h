#pragma once

#ifdef WIN32
#include <Windows.h>
#endif

#include <thread>
#include <chrono>

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

	class FpsLimiter
	{
	public:
		FpsLimiter(int targetMs)
		{
			m_targetDuration = std::chrono::milliseconds(targetMs);
			m_endFrameTime = std::chrono::steady_clock::now();
			m_startFrameTime = std::chrono::steady_clock::now();
		}

		void NewFrame()
		{
			m_startFrameTime = std::chrono::steady_clock::now();
			auto deltaTimeChrono = m_startFrameTime - m_endFrameTime;

			std::this_thread::sleep_for(m_targetDuration - deltaTimeChrono);

			m_endFrameTime = std::chrono::steady_clock::now();
		}

	private:
		std::chrono::milliseconds m_targetDuration;
		std::chrono::steady_clock::time_point m_startFrameTime;
		std::chrono::steady_clock::time_point m_endFrameTime;
	};
}
