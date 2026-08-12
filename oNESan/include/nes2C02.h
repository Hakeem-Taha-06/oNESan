#pragma once

#include <cstdint>

class nes2C02
{
public:
	uint8_t cpuRead(uint16_t addr);
	void cpuWrite(uint16_t addr, uint8_t data);
};

