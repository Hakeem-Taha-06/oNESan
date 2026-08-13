#pragma once

#include <vector>
#include <cstdint>
#include <string>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <memory>

#include "Mapper.h"
#include "Mapper000.h"

class Cartridge
{
public:
	Cartridge(std::string path);
	~Cartridge();

	uint8_t cpuRead(uint16_t addr);
	void cpuWrite(uint16_t addr, uint8_t data);

	uint8_t ppuRead(uint16_t addr);
	void ppuWrite(uint16_t addr, uint8_t data);

private:
	std::vector<uint8_t> PRGROM;
	std::vector<uint8_t> CHRROM;

	uint8_t numPRGBanks, numCHRBanks;
	uint8_t mapperID;

	std::shared_ptr<Mapper> mapper;
};

