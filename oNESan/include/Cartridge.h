#pragma once

#include <vector>
#include <cstdint>
#include <string>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <memory>

#include "Mappers/Mapper.h"
#include "Mappers/Mapper000.h"
#include "Mappers/Mapper003.h"

class Cartridge
{
public:
	Cartridge(std::string path);
	~Cartridge();

	bool cpuRead(uint16_t addr, uint8_t& data);
	bool cpuWrite(uint16_t addr, uint8_t data);

	bool ppuRead(uint16_t addr, uint8_t& data);
	bool ppuWrite(uint16_t addr, uint8_t data);

	inline size_t getPRGSize() const { return PRGROM.size(); }
	inline size_t getCHRSize() const { return CHRROM.size(); }

	inline const std::vector<uint8_t>& getPRGROM() const { return PRGROM; }
	inline const std::vector<uint8_t>& getCHRROM() const { return CHRROM; }

	enum Mirroring {
		VERTICAL,
		HORIZONTAL, 
		ONESCREEN_LO,
		ONESCREEN_HI
	} mirroring = HORIZONTAL;
private:
	std::vector<uint8_t> PRGROM;
	std::vector<uint8_t> CHRROM;

	uint8_t numPRGBanks, numCHRBanks;
	uint8_t mapperID;

	std::shared_ptr<Mapper> mapper;
};

