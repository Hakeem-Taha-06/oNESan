#pragma once

#include <cstdint>
#include <array>
#include "nes6502.h"

class Bus
{
public:
	Bus();
	~Bus();

	uint8_t read(uint16_t address);
	void write(uint16_t address, uint8_t data);

	//temporary ram
	std::array<uint8_t, 64 * 1028> ram;
	nes6502 nes6502;
};

