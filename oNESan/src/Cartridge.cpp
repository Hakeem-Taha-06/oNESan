#include "Cartridge.h"

Cartridge::Cartridge(std::string path) {
	std::ifstream file{ path, std::ios::binary };
	
	if (!file) 
		throw std::runtime_error("Couldn't read file: " + path);
	
	union iNESHeader {
		struct {
			uint8_t NES[4];    // constant, contains the characters 'N', 'E', 'S' and an EOF '0x1a'
			uint8_t PRGSize;   // in 16KB units (ex. 1 -> 1 * 16KBS,  2 -> 2*16KBS)
			uint8_t CHRSize;   // in 8KB units (if 0, then PRGRAM is used)
			uint8_t mapper1;   // mapper flags first 8 bits
			uint8_t mapper2;   // mapper flags last 8 bits
			uint8_t PRGRamSize;
			uint8_t TVSystem1;
			uint8_t TVSystem2;
			uint8_t unused[5];
		};
		uint8_t data[16];
	} header{};
	
	// read the first 16 bytes as the header
	file.read((char*)header.data, 16);
	
	// mapper1's 4 leftmost bits contain the lowest 4 bits, mapper2's 4 leftmost bits contain the highest 4 bits 
	mapperID = (header.mapper1 >> 4) | (header.mapper2 & 0xF0); 
	mirroring = header.mapper1 & 0x01 ? Mirroring::VERTICAL : Mirroring::HORIZONTAL;
	numPRGBanks = header.PRGSize;
	numCHRBanks = header.CHRSize;
	// choose the proper mapper
	switch (mapperID) {
	case 0x0000:
		mapper = std::make_shared<Mapper000>(numPRGBanks, numCHRBanks);
		break;
	}
	
	// skip past the trainer
	if (header.mapper1 & 0x04) 
		file.seekg(512, std::ios::right);
	
	// read the PRG ROM and CHR ROM data based on the size defined in the header
	PRGROM.resize(numPRGBanks * 16384);
	file.read((char*)PRGROM.data(), PRGROM.size());
	CHRROM.resize(numCHRBanks * 8192);
	file.read((char*)CHRROM.data(), CHRROM.size());

	file.close();
}

Cartridge::~Cartridge() {

}


bool Cartridge::cpuRead(uint16_t addr, uint8_t& data) {

	uint16_t mappedAddr = 0x0000;

	if (mapper->cpuMapRead(addr, mappedAddr)) {
		data = PRGROM[mappedAddr];
		return true;
	}

	return false;
}
bool Cartridge::cpuWrite(uint16_t addr, uint8_t data) {
	uint16_t mappedAddr = 0x0000;

	if (mapper->cpuMapWrite(addr, mappedAddr)) {
		PRGROM[mappedAddr] = data;
		return true;
	}

	return false;
}

bool Cartridge::ppuRead(uint16_t addr, uint8_t& data) {

	uint16_t mappedAddr = 0x0000;
	if (mapper->ppuMapRead(addr, mappedAddr)) {
		data = CHRROM[mappedAddr];
		return true;
	}
	
	return false;
}

bool Cartridge::ppuWrite(uint16_t addr, uint8_t data) {
	
	uint16_t mappedAddr = 0x0000;
	if (mapper->ppuMapWrite(addr, mappedAddr)) {
		CHRROM[mappedAddr] = data;
		return true;
	}

	return false;
}