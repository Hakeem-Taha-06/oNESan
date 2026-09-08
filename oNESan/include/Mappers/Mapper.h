#pragma once

#include <cstdint>

// Base Mapper Interface
class Mapper
{
public:
	Mapper(uint8_t PRGBanksN, uint8_t CHRBanksN)
		:numPRGBanks(PRGBanksN), numCHRBanks(CHRBanksN)
	{
		
	};

	virtual bool cpuMapRead(uint16_t addr, uint32_t& mappedAddr) = 0;
	virtual bool cpuMapWrite(uint16_t addr, uint32_t& mappedAddr, uint8_t data) = 0;
	virtual bool ppuMapRead(uint16_t addr, uint32_t& mappedAddr) = 0;
	virtual bool ppuMapWrite(uint16_t addr, uint32_t& mappedAddr, uint8_t data) = 0;

protected:
	uint8_t numPRGBanks, numCHRBanks;
};

