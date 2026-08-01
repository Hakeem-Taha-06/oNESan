#pragma once

#include <iostream>

#include "Bus.h"
#include "nes6502.h"

class Emulator {
public:
	Emulator();
	~Emulator();

	void init();
	void clock();

	void writeTestProgram();

	Bus bus;

	uint8_t testOperand = 0x00;
};