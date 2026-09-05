#pragma once

#include <cstdint>

#include "Mapper.h"

class Mapper000 : public Mapper
{
public:
	Mapper000(int PRGBanksN, int CHRBanksN);
	~Mapper000();

	//uint16_t cpuMapAddress(uint16_t addr) override;
	//uint16_t ppuMapAddress(uint16_t addr) override;

	bool cpuMapRead(uint16_t addr, uint16_t& mappedAddr) override;
	bool cpuMapWrite(uint16_t addr, uint16_t& mappedAddr) override;
	bool ppuMapRead(uint16_t addr, uint16_t& mappedAddr) override;
	bool ppuMapWrite(uint16_t addr, uint16_t& mappedAddr) override;
};

