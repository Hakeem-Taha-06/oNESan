#pragma once

#include <iostream>

#include "Bus.h"
#include "nes6502.h"
#include "Window.h"

class Emulator {
public:
	Emulator();
	~Emulator();

	void run();
private:
	void init();

	Bus* bus;
	nes6502* cpu;
	Window* window;
};