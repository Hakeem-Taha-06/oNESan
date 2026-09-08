#pragma once

#include <cstdint>
#include <vector>

#include "Mappers/Mapper.h"

class Mapper003 : public Mapper
{
public:
	Mapper003(uint8_t numPRGBanks, uint8_t numCHRBanks, const std::vector<uint8_t>* prgRomRef);
	~Mapper003();

	bool cpuMapRead(uint16_t addr, uint32_t& mappedAddr) override;
	bool cpuMapWrite(uint16_t addr, uint32_t& mappedAddr, uint8_t data) override;
	bool ppuMapRead(uint16_t addr, uint32_t& mappedAddr) override;
	bool ppuMapWrite(uint16_t addr, uint32_t& mappedAddr, uint8_t data) override;

private:
	uint8_t CHRROMBank = 0x00;
	const std::vector<uint8_t>* prgRom;

};

