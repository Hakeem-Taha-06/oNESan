#pragma once

#include <fstream>
#include <vector>
#include <cstdint>
#include <iostream>
#include <stdexcept>

#include "Bus.h"
#include "nes6502.h"

class Emulator {
public:
	Emulator();
	~Emulator();

	void init();
	void clock();
	void load(const std::string& path);

	TestBus bus;
};