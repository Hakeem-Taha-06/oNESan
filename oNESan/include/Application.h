#pragma once

#include "Emulator.h"
#include "Window.h"

#include <chrono>
#include <iostream>

const unsigned int NES_CPU_HZ = 1789773;

class Application
{
public:
	Application(int width, int height);
	~Application();

	void run();

private:
	void init();

	Emulator emulator;
	Window window;

	std::chrono::steady_clock::time_point lastTime;

#ifdef DEBUG

	double debugOutTimeLimit = 1.0;
	double debugOutTime = 0.0;

	int cyclesEmulated = 0;
#endif // DEBUG
};

