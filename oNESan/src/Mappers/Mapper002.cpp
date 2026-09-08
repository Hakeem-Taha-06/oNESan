#include "Mappers/Mapper002.h"


Mapper002::Mapper002(uint8_t numPRGBanks, uint8_t numCHRBanks) : Mapper(numPRGBanks, numCHRBanks){

}
Mapper002::~Mapper002() {

}

bool Mapper002::cpuMapRead(uint16_t addr, uint32_t& mappedAddr){
	if (addr >= 0x8000 && addr <= 0xBFFF) { // switchable 16 kb banks
		mappedAddr = PRGROMBank * 0x4000 + (addr & 0x3FFF);
		return true;
	}
	else if (addr >= 0xC000 && addr <= 0xFFFF) { // last bank
		mappedAddr = (numPRGBanks - 1) * 0x4000 + (addr & 0x3FFF);
		return true;
	}

	return false;
}
bool Mapper002::cpuMapWrite(uint16_t addr, uint32_t& mappedAddr, uint8_t data) {

	if (addr >= 0x8000 && addr <= 0xFFFF) {
		PRGROMBank = data & (numPRGBanks - 1);
	}

	return false;
}
bool Mapper002::ppuMapRead(uint16_t addr, uint32_t& mappedAddr) {
	if (addr >= 0x0000 && addr <= 0x1FFF) {
		mappedAddr = addr;
		return true;
	}

	return false;
}
bool Mapper002::ppuMapWrite(uint16_t addr, uint32_t& mappedAddr, uint8_t data) {
	if (addr >= 0x0000 && addr <= 0x1FFF) {
		if (numCHRBanks == 0) {
			mappedAddr = addr;
			return true;
		}
	}

	return false;
}