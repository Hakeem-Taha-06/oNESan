#pragma once

#include <fstream>
#include <vector>
#include <cstdint>
#include <iostream>
#include <stdexcept>
#include <memory>

#include "Bus.h"
#include "nes6502.h"
#include "nes2C02.h"
#include "Cartridge.h" 

class Emulator {
public:
	Emulator();
	~Emulator();

	void reset();
	void clock();
	void load(std::string path);

	void loadCartridge(std::string path);

	NesBus bus;
	std::shared_ptr<Cartridge> cart;
};