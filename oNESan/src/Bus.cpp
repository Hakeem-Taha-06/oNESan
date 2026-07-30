#include "Bus.h"

Bus::Bus() {
	cpu.connectBus(this);

	for (auto& i : ram) i = 0x00;
}

Bus::~Bus() {

}

uint8_t Bus::read(uint16_t address) {
	if(address >= 0x0000 && address <= 0xFFFE)
		return ram[address];

	return 0x00;  
}

void Bus::write(uint16_t address, uint8_t data) {
	if (address >= 0x0000 && address <= 0xFFFE)
		ram[address] = data;
}