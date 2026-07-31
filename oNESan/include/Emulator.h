#pragma once

#include <iostream>

#include "Bus.h"
#include "nes6502.h"

class Emulator {
public:
	Emulator();
	~Emulator();

	void clock();
	void runTestProgram();
private:
	void init();

	Bus bus;
};