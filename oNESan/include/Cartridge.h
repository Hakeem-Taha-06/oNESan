#pragma once

#include <vector>
#include <cstdint>
#include <string>
#include <fstream>
#include <sstream>


class Cartridge
{
public:
	Cartridge(std::string path);
	~Cartridge();

	uint8_t cpuRead(uint16_t addr);
	void cpuWrite(uint16_t addr, uint8_t data);

private:
	std::vector<uint8_t> PRG_ROM;
	std::vector<uint8_t> CHR_ROM;
};

