#include "Mappers/Mapper000.h"

Mapper000::Mapper000(uint8_t numPRGBanks, uint8_t numCHRBanks): Mapper(numPRGBanks, numCHRBanks) {

}

Mapper000::~Mapper000() {

}

bool Mapper000::cpuMapRead(uint16_t addr, uint16_t& mappedAddr){
	if (addr >= 0x8000 && addr <= 0xFFFF) {
		mappedAddr = addr & (numPRGBanks > 1 ? 0x7FFF : 0x3FFF);
		return true;
	}

	return false;
}

bool Mapper000::cpuMapWrite(uint16_t addr, uint16_t& mappedAddr, uint8_t data){
	// PRGROM is not writable
	//if (addr >= 0x8000 && addr <= 0xFFFF) {
	//	mappedAddr = addr & (numPRGBanks > 1 ? 0x7FFF : 0x3FFF);
	//	return true;
	//}

	return false;
}

bool Mapper000::ppuMapRead(uint16_t addr, uint16_t& mappedAddr){
	if (addr >= 0x0000 && addr <= 0x1FFF) {
		mappedAddr = addr;
		return true;
	}

	return false;
}

bool Mapper000::ppuMapWrite(uint16_t addr, uint16_t& mappedAddr, uint8_t data) {
	if (addr >= 0x0000 && addr <= 0x1FFF) {
		if (numCHRBanks == 0) {
			mappedAddr = addr;
			return true;
		}
	}

	return false;
}
