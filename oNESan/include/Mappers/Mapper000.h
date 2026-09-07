#pragma once

#include <cstdint>

#include "Mappers/Mapper.h"

class Mapper000 : public Mapper
{
public:
	Mapper000(uint8_t numPRGBanks, uint8_t numCHRBanks);
	~Mapper000();

	bool cpuMapRead(uint16_t addr, uint16_t& mappedAddr) override;
	bool cpuMapWrite(uint16_t addr, uint16_t& mappedAddr, uint8_t data) override;
	bool ppuMapRead(uint16_t addr, uint16_t& mappedAddr) override;
	bool ppuMapWrite(uint16_t addr, uint16_t& mappedAddr, uint8_t data) override;
};

