#include "Mapper000.h"

Mapper000::Mapper000(int PRGBanksN, int CHRBanksN): Mapper(PRGBanksN, CHRBanksN) {

}

Mapper000::~Mapper000() {

}

uint16_t Mapper000::cpuMapAddress(uint16_t addr){
	uint16_t mappedAddr = addr & (numPRGBanks > 1 ? 0x7FFF : 0x3FFF);


	return mappedAddr;
}
uint16_t Mapper000::ppuMapAddress(uint16_t addr) {
	uint16_t mappedAddr = addr;


	return mappedAddr;
}