#include "Mappers/Mapper003.h"


Mapper003::Mapper003(uint8_t numPRGBanks, uint8_t numCHRBanks, const std::vector<uint8_t>* prgRomRef) : Mapper(numPRGBanks, numCHRBanks), prgRom(prgRomRef) {

}

Mapper003::~Mapper003() {

}

bool Mapper003::cpuMapRead(uint16_t addr, uint16_t& mappedAddr) {
	// PRG RAM
	//if (addr >= 0x6000 && addr < 0x7FFF) {
	//	mappedAddr = addr & 0x07FF; // should signal it is in prg ram
	//	return true;
	//}
	if(addr >= 0x8000 && addr <= 0xFFFF){
		mappedAddr = addr & (numCHRBanks > 1 ? 0x7FFF : 0x3FFF);
		return true;
	}

	return false;
}

bool Mapper003::cpuMapWrite(uint16_t addr, uint16_t& mappedAddr, uint8_t data) {
	
	// PRG RAM
	//if (addr >= 0x6000 && addr < 0x7FFF) {
	//	mappedAddr = addr & 0x07FF;
	//	return true;
	//}

	if (addr >= 0x8000 && addr <= 0xFFFF) {
		// bus conflicts, only used by some ROMs
		//CHRROMBank = ((*prgRom)[addr] & data) & 0x03;

		CHRROMBank = data & (numCHRBanks-1);
	}

	return false;
}

bool Mapper003::ppuMapRead(uint16_t addr, uint16_t& mappedAddr) {
	if (addr >= 0x0000 && addr <= 0x1FFF) {
		mappedAddr = CHRROMBank * 0x2000 + addr;
		return true;
	}

	return false;
}

bool Mapper003::ppuMapWrite(uint16_t addr, uint16_t& mappedAddr, uint8_t data) {
	if (addr >= 0x0000 && addr <= 0x1FFF) {
		if (numCHRBanks == 0) {
			mappedAddr = CHRROMBank * 0x2000 + addr;
			return true;
		}
	}

	return false;
}
