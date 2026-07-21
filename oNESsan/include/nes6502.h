#pragma once
#include <cstdint>

class Bus;

class nes6502
{
public:
	void connectBus(Bus* bus);

private:
	Bus* bus = nullptr;

	uint8_t read(uint16_t address);
	void write(uint16_t address, uint8_t data);
};

