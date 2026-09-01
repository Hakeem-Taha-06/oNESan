#pragma once

#include <cstdint>
#include <array>
#include <memory>

#include "nes6502.h"
#include "nes2C02.h"
#include "Cartridge.h"
#include "Controller.h"

class Bus
{
public:
	virtual ~Bus() = default;

	virtual uint8_t cpuRead(uint16_t address) = 0;
	virtual void cpuWrite(uint16_t address, uint8_t data) = 0;

	virtual void clock() = 0;
	virtual void reset() = 0;
};

class TestBus : public Bus
{
public:
	TestBus();
	~TestBus();

	uint8_t cpuRead(uint16_t address) override;
	void cpuWrite(uint16_t address, uint8_t data) override;

	void clock() override;
	void reset() override;

	// temporary ram
	std::array<uint8_t, 64 * 1024> ram;
	nes6502 cpu;
};

class NesBus : public Bus
{
public:
	NesBus();
	~NesBus();

	uint8_t cpuRead(uint16_t addr) override;
	void cpuWrite(uint16_t addr, uint8_t data) override;

	void insertCartridge(const std::shared_ptr<Cartridge>& cartridge);

	void clock() override;
	void reset() override;

	nes6502 cpu;
	nes2C02 ppu;
	std::shared_ptr<Cartridge> cart;
	Controller joypad;

	std::array<uint8_t, 2 * 1024> cpuRam;

	int systemClockCounter = 0;

private:
	uint8_t dmaPage = 0x00;
	uint8_t dmaAddr = 0x00;
	uint8_t dmaData = 0x00;
	bool dmaTransfer = false;
	bool dmaDummy = false;
};
