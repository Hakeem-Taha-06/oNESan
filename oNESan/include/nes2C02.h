#pragma once

#include <cstdint>
#include <memory>

class Cartridge;

class nes2C02
{
public:

	void clock();

	uint8_t cpuRead(uint16_t addr);
	void cpuWrite(uint16_t addr, uint8_t data);

	// ppu bus
	uint8_t ppuRead(uint16_t addr);
	void ppuWrite(uint16_t addr, uint8_t data);

	void insertCartridge(const std::shared_ptr<Cartridge>& cartridge);

	std::shared_ptr<Cartridge> cart;
};

