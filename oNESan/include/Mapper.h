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

	virtual uint16_t cpuMapAddress(uint16_t addr) = 0;
	virtual uint16_t ppuMapAddress(uint16_t addr) = 0;

protected:
	int numPRGBanks, numCHRBanks;
};

