#pragma once

#include <cstdint>
#include <array>
#include <memory>

#include "nes6502.h"
#include "nes2C02.h"
#include "Cartridge.h"

class TestBus
{
public:
	TestBus();
	~TestBus();

	uint8_t cpuRead(uint16_t address);
	void cpuWrite(uint16_t address, uint8_t data);

	// temporary ram
	std::array<uint8_t, 64 * 1024> ram;
	nes6502 cpu;
};

class NesBus 
{
public:
	NesBus();
	~NesBus();

	uint8_t cpuRead(uint16_t addr);
	void cpuWrite(uint16_t addr, uint8_t data);

	void insertCartridge(const std::shared_ptr<Cartridge>& cartridge);

	nes6502 cpu;
	nes2C02 ppu;
	std::shared_ptr<Cartridge> cart;

	std::array<uint8_t, 2 * 1024> cpuRam;

};
