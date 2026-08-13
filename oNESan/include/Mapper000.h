#pragma once

#include <cstdint>

#include "Mapper.h"

class Mapper000 : public Mapper
{
public:
	Mapper000(int PRGBanksN, int CHRBanksN);
	~Mapper000();

	uint16_t cpuMapAddress(uint16_t addr) override;
	uint16_t ppuMapAddress(uint16_t addr) override;
};

