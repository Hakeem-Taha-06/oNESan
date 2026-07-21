#include "nes6502.h"
#include "Bus.h"

void nes6502::connectBus(Bus* b) {
	bus = b;
}

uint8_t nes6502::read(uint16_t address) {
	return bus->read(address);
}
void nes6502::write(uint16_t address, uint8_t data) {
	bus->write(address, data);
}