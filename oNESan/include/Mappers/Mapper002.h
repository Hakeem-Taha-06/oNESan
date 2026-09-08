#pragma once

#include <Mappers/Mapper.h>

class Mapper002 : public Mapper
{
public:
	Mapper002(uint8_t numPRGBanks, uint8_t numCHRBanks);
	~Mapper002();

	bool cpuMapRead(uint16_t addr, uint32_t& mappedAddr) override;
	bool cpuMapWrite(uint16_t addr, uint32_t& mappedAddr, uint8_t data) override;
	bool ppuMapRead(uint16_t addr, uint32_t& mappedAddr) override;
	bool ppuMapWrite(uint16_t addr, uint32_t& mappedAddr, uint8_t data) override;

private:
	uint8_t PRGROMBank = 0x00;
};

