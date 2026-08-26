#include "nes2C02.h"
#include "Cartridge.h"

nes2C02::nes2C02() {
	for (int i = 0; i < 32;++i) {
		palettes[i] = 0x27; // hardcoded for testing
	}
}

void nes2C02::clock() {

	cycle++;
	if (cycle >= 340) {
		cycle = 0;

		scanline++;
		if (scanline == 241) {
			//vblank
		}
		if (scanline >= 260) {
			scanline = -1;
		}
	}
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

uint32_t nes2C02::getColorFromPalette(int palette, int pixel) {
	return NES_PALETTE[ppuRead(0x3F00 + ((palette * 4 + pixel) & 0x3F))];
}

// parses the contents of CHRROM and returns the screen pixel data representation
float* nes2C02::getPatternTable(int i, int palette) {
	int screenIdx = 0;
	for (uint16_t tileY = 0; tileY < 16; ++tileY) {
		for (uint16_t tileX = 0; tileX < 16; ++tileX) {
			uint16_t offset = tileY * 256 + tileX * 16;
			
			for (uint16_t row = 0; row < 8; ++row) {
				uint8_t lsbitplane = ppuRead(i * 0x1000 + offset + row);
				uint8_t msbitplane = ppuRead(i * 0x1000 + offset + row + 8);

				for (uint16_t col = 0; col < 8; ++col) {
					uint8_t pixel = (lsbitplane & 0x01) | ((msbitplane & 0x01) << 1);
					lsbitplane >>= 1; msbitplane >>= 1;

					// divide by 255 to normalize for use by rendering API (openGL)
					patternScreen[i][screenIdx++] = (float)(getColorFromPalette(palette, pixel) >> 16 & 0x0000FF) / 255 ; // 0xRRGGBB
					patternScreen[i][screenIdx++] = (float)(getColorFromPalette(palette, pixel) >> 8  & 0x0000FF) / 255 ;
					patternScreen[i][screenIdx++] = (float)(getColorFromPalette(palette, pixel) >> 0  & 0x0000FF) / 255 ;
				}
			}
		}
	}
	return patternScreen[i];
}
//---------------------------------- PPU BUS

void nes2C02::insertCartridge(const std::shared_ptr<Cartridge>& cartridge) {
	cart = cartridge;
}

uint8_t nes2C02::ppuRead(uint16_t addr) {
	uint8_t data = 0x00;
	addr &= 0x3FFF;

	// Mapped by cartridge
	if (addr >= 0x0000 && addr <= 0x1FFF) {
		data = cart->ppuRead(addr);
	}
	if (addr >= 2000 && addr <= 0x3EFF) {

	}
	if (addr >= 0x3F00 && addr <= 0x3FFF) {
		addr &= 0x001F;
		if (addr == 0x0004 ||
			addr == 0x0008 ||
			addr == 0x000C ||
			addr == 0x0010 ||
			addr == 0x0014 ||
			addr == 0x0018 ||
			addr == 0x001C)  addr = 0x0000;

		data = palettes[addr];
	}

	return data;
}

void nes2C02::ppuWrite(uint16_t addr, uint8_t data) {
	if (addr >= 0x0000 && addr <= 0x1FFF) {
		cart->ppuWrite(addr, data);
	}
}
