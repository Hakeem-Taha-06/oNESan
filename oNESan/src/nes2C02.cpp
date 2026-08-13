#include "nes2C02.h"
#include "Cartridge.h"

void nes2C02::clock() {

}

uint8_t nes2C02::cpuRead(uint16_t addr) {
	uint8_t data = 0x00;
	switch (addr) {
		case 0:    // CTRL
			break;
		case 1:    // MASK
			break;
		case 2:    // STATUS
			break;
		case 3:    // OAMADDR
			break;
		case 4:    // OAMDATA
			break;
		case 5:    // SCROLL
			break;
		case 6:    // PPUADDR
			break;
		case 7:    // PPUDATA
			break;
	}
	return data;
}

void nes2C02::cpuWrite(uint16_t addr, uint8_t data) {
	switch (addr) {
		case 0:    // CTRL
			break;
		case 1:    // MASK
			break;
		case 2:    // STATUS
			break;
		case 3:    // OAMADDR
			break;
		case 4:    // OAMDATA
			break;
		case 5:    // SCROLL
			break;
		case 6:    // PPUADDR
			break;
		case 7:    // PPUDATA
			break;
	}
}


//---------------------------------- PPU BUS

void nes2C02::insertCartridge(const std::shared_ptr<Cartridge>& cartridge) {
	cart = cartridge;
}

uint8_t nes2C02::ppuRead(uint16_t addr) {
	uint8_t data = 0x00;
	// Mapped by cartridge
	if (addr >= 0x0000 && addr <= 0x1FFF) {
		data = cart->ppuRead(addr);
	}

	return data;
}

void nes2C02::ppuWrite(uint16_t addr, uint8_t data) {
	if (addr >= 0x0000 && addr <= 0x1FFF) {
		cart->ppuWrite(addr, data);
	}
}
