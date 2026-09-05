#pragma once

#include <cstdint>

// Base Mapper Interface
class Mapper
{
public:
	Mapper(int PRGBanksN, int CHRBanksN)
		:numPRGBanks(PRGBanksN), numCHRBanks(CHRBanksN)
	{
		
	};

	virtual bool cpuMapRead(uint16_t addr, uint16_t& mappedAddr) = 0;
	virtual bool cpuMapWrite(uint16_t addr, uint16_t& mappedAddr) = 0;
	virtual bool ppuMapRead(uint16_t addr, uint16_t& mappedAddr) = 0;
	virtual bool ppuMapWrite(uint16_t addr, uint16_t& mappedAddr) = 0;

protected:
	int numPRGBanks, numCHRBanks;
};

