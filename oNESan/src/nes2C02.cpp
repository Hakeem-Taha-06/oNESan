#include "nes2C02.h"
#include "Cartridge.h"

nes2C02::nes2C02() {

}

void nes2C02::clock() {

	auto incrementScrollX = [&]() {
			if (mask.renderBG || mask.renderSprites) {
				// wrap around to beginning of next nametable
				if (vramAddr.coarseX == 31) {
					vramAddr.coarseX = 0;
					// flip nametable bit to move to the next nametable
					vramAddr.nametableX = ~vramAddr.nametableX;
				}
				else {
					vramAddr.coarseX++;
				}
			}
		};

	auto incrementScrollY = [&]() {
			if (mask.renderBG || mask.renderSprites) {
				if (vramAddr.fineY < 7) {
					vramAddr.fineY++;
				}
				else {
					vramAddr.fineY = 0;

					if (vramAddr.coarseY == 29) {
						vramAddr.coarseY = 0;
						vramAddr.nametableY = ~vramAddr.nametableY;
					}
					else if (vramAddr.coarseY == 31) {
						vramAddr.coarseY = 0;
					}
					else {
						vramAddr.coarseY++;
					}
				}
			}

		};

	auto transferAddressX = [&]() {
			if (mask.renderBG || mask.renderSprites) {
				vramAddr.nametableX = tramAddr.nametableX;
				vramAddr.coarseX = tramAddr.coarseX;
			}
		};

	auto transferAddressY = [&]() {
			if (mask.renderBG || mask.renderSprites) {
				vramAddr.nametableY = tramAddr.nametableY;
				vramAddr.coarseY = tramAddr.coarseY;
			}
		};

	// Shifters 
	auto loadBackgroundShifters = [&]() {
			bgShifterPatternLo = (bgShifterPatternLo & 0xFF00) | bgNextTileLsb;
			bgShifterPatternHi = (bgShifterPatternHi & 0xFF00) | bgNextTileMsb;

			bgShifterAttribLo = (bgShifterAttribLo & 0xFF00) | (bgNextTileAttrib & 0b01 ? 0xFF : 0x00);
			bgShifterAttribHi = (bgShifterAttribHi & 0xFF00) | (bgNextTileAttrib & 0b10 ? 0xFF : 0x00);
		};

	auto updateShifters = [&]() {
			if (mask.renderBG) {
				bgShifterPatternLo <<= 1;
				bgShifterPatternHi <<= 1;

				bgShifterAttribLo <<= 1;
				bgShifterAttribHi <<= 1;
			}
		};

	// visible scanlines
	if (scanline >= -1 && scanline < 240) {
		if (scanline == -1 && cycle == 1) {
			status.verticalBlank = 0;
		}

		if ((cycle >= 2 && cycle < 258) || (cycle >= 321 && cycle < 338)) {

			updateShifters();

			switch ((cycle - 1) % 8) {
				case 0:
					loadBackgroundShifters();
					// reads a byte from the nametable using the v address
					bgNextTileID = ppuRead(0x2000 | vramAddr.reg & 0x0FFF);
					break;
				case 2:
					// reads a byte from the attribute table (i will never be able to understand this line ever again)
					bgNextTileAttrib = ppuRead(0x23C0 | (vramAddr.nametableY << 11)
													  | (vramAddr.nametableX << 10)
													  | ((vramAddr.coarseY >> 2) << 3)
													  | (vramAddr.coarseX >> 2));
					if (vramAddr.coarseY & 0x02) bgNextTileAttrib >>= 4;
					if (vramAddr.coarseX & 0x02) bgNextTileAttrib >>= 2;
					bgNextTileAttrib &= 0x03;
					break;
				case 4:
					bgNextTileLsb = ppuRead((control.patternBG << 12) + (bgNextTileID << 4) + vramAddr.fineY);
					break;
				case 6:
					bgNextTileMsb = ppuRead((control.patternBG << 12) + (bgNextTileID << 4) + vramAddr.fineY + 8);
					break;
				case 7:
					incrementScrollX();
					break;
			}
		}

		if (cycle == 256) {
			incrementScrollY();
		}

		if (cycle == 257) {
			transferAddressX();
		}

		if (scanline == -1 && cycle >= 280 && cycle < 305) {
			transferAddressY();
		}
	}

	if (scanline == 240) {
		// nothing happens here
	}

	if (scanline == 241 && cycle == 1) {
		status.verticalBlank = 1;
		if (control.enableNmi) emitNmi = true;
	}

	uint8_t bgPixel = 0x00;
	uint8_t bgPalette = 0x00;
	if (mask.renderBG) {
		uint16_t bitMux = 0x8000 >> fineX;
		uint8_t pixel0 = (bgShifterPatternLo & bitMux) > 0;
		uint8_t pixel1 = (bgShifterPatternHi & bitMux) > 0;
		bgPixel = (pixel1 << 1) | pixel0;

		uint8_t pal0 = (bgShifterAttribLo & bitMux) > 0;
		uint8_t pal1 = (bgShifterAttribHi & bitMux) > 0;
		bgPalette = (pal1 << 1) | pal0;
	}

	if (scanline >= 0 && scanline < 240 && cycle >= 1 && cycle <= 256) {
		int idx = (scanline * 256 + cycle - 1) * 3;
		uint32_t color = getColorFromPalette(bgPalette, bgPixel);
		screen[idx + 0] = (float)(color >> 16 & 0x0000FF) / 255;
		screen[idx + 1] = (float)(color >> 8 & 0x0000FF) / 255;
		screen[idx + 2] = (float)(color >> 0 & 0x0000FF) / 255;
	}

	cycle++;
	if (cycle >= 341) {
		cycle = 0;

		scanline++;
		
		if (scanline >= 261) {
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
			data = (status.reg & 0xE0) | (ppuDataBuffer & 0x1F); // top three bits from the status register + noise from the data buffer
			status.verticalBlank = 0;
			addressLatch = 0;
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
			data = ppuDataBuffer;
			ppuDataBuffer = ppuRead(vramAddr.reg);

			// palette memory reads don't get delayed 
			if (vramAddr.reg >= 0x3F00) data = ppuDataBuffer;
			vramAddr.reg += control.incrementMode ? 32 : 1;
			break;
	}
	return data;
}

void nes2C02::cpuWrite(uint16_t addr, uint8_t data) {
	switch (addr) {
		case 0:    // CTRL
			control.reg = data;
			tramAddr.nametableX = control.nametableX;
			tramAddr.nametableY = control.nametableY;
			break;
		case 1:    // MASK
			mask.reg = data;
			break;
		case 2:    // STATUS
			break;
		case 3:    // OAMADDR
			break;
		case 4:    // OAMDATA
			break;
		case 5:    // SCROLL
			if (addressLatch == 0) {
				// fine x takes the bottom three bits, coarse x takes the rest
				fineX = data & 0x07;
				tramAddr.coarseX = data >> 3;
				addressLatch = 1;
			}
			else {
				tramAddr.fineY = data & 0x07;
				tramAddr.coarseY = data >> 3;
				addressLatch = 0;
			}
			break;
		case 6:    // PPUADDR
			if (addressLatch == 0) { 
				// Big Endian, first byte is the high byte
				tramAddr.reg = (tramAddr.reg & 0x00FF) | (data << 8);
				addressLatch = 1;
			}
			else {
				// second byte is the low byte
				tramAddr.reg = (tramAddr.reg & 0xFF00) | data;
				vramAddr = tramAddr;
				addressLatch = 0;
			}
			break;
		case 7:    // PPUDATA
			ppuWrite(vramAddr.reg, data);
			vramAddr.reg += control.incrementMode ? 32 : 1;
			break;
	}
}

uint32_t nes2C02::getColorFromPalette(uint8_t palette, uint8_t pixel) {
	return NES_PALETTE[ppuRead(0x3F00 + (palette << 2) + pixel) & 0x3F];
}

// parses the contents of CHRROM and returns the screen pixel data representation
float* nes2C02::getPatternTable(uint8_t i, uint8_t palette) {
	for (uint16_t tileY = 0; tileY < 16; ++tileY) {
		for (uint16_t tileX = 0; tileX < 16; ++tileX) {
			uint16_t offset = tileY * 256 + tileX * 16;
			
			for (uint16_t row = 0; row < 8; ++row) {
				uint8_t lsbitplane = ppuRead(i * 0x1000 + offset + row);
				uint8_t msbitplane = ppuRead(i * 0x1000 + offset + row + 8);

				for (uint16_t col = 0; col < 8; ++col) {
					uint8_t pixel = (lsbitplane & 0x01) | ((msbitplane & 0x01) << 1);
					lsbitplane >>= 1; msbitplane >>= 1;

					int x = tileX * 8 + (7 - col);
					int y = tileY * 8 + row;
					int idx = (y * 128 + x) * 3;

					uint32_t color = getColorFromPalette(palette, pixel);
					// divide by 255 to normalize for use by rendering API (openGL)
					patternScreen[i][idx + 0] = (float)(color >> 16 & 0x0000FF) / 255; // 0xRRGGBB
					patternScreen[i][idx + 1] = (float)(color >> 8  & 0x0000FF) / 255;
					patternScreen[i][idx + 2] = (float)(color >> 0  & 0x0000FF) / 255;
				}
			}
		}
	}
	return patternScreen[i];
}

const float* nes2C02::getScreen() {
	return screen;
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
	else if (addr >= 0x02000 && addr <= 0x3EFF) {
		addr &= 0x0FFF;
		if (cart->mirroring == Cartridge::Mirroring::VERTICAL) {
			if (addr >= 0x0000 && addr <= 0x03FF)
				data = nametables[0][addr & 0x03FF];
			if (addr >= 0x0400 && addr <= 0x07FF)
				data = nametables[1][addr & 0x03FF];
			if (addr >= 0x0800 && addr <= 0x0BFF)
				data = nametables[0][addr & 0x03FF];
			if (addr >= 0x0C00 && addr <= 0x0EFF)
				data = nametables[1][addr & 0x03FF];
		}
		else if (cart->mirroring == Cartridge::Mirroring::HORIZONTAL) {
			if (addr >= 0x0000 && addr <= 0x03FF)
				data = nametables[0][addr & 0x03FF];
			if (addr >= 0x0400 && addr <= 0x07FF)
				data = nametables[0][addr & 0x03FF];
			if (addr >= 0x0800 && addr <= 0x0BFF)
				data = nametables[1][addr & 0x03FF];
			if (addr >= 0x0C00 && addr <= 0x0EFF)
				data = nametables[1][addr & 0x03FF];
		}
	}
	else if (addr >= 0x3F00 && addr <= 0x3FFF) {
		addr &= 0x001F;
		if (addr == 0x0010) addr = 0x0000;
		if (addr == 0x0014) addr = 0x0004;
		if (addr == 0x0018) addr = 0x0008;
		if (addr == 0x001C) addr = 0x000C;

		data = palettes[addr];
	}

	return data;
}

void nes2C02::ppuWrite(uint16_t addr, uint8_t data) {
	if (addr >= 0x0000 && addr <= 0x1FFF) {
		cart->ppuWrite(addr, data);
	}
	else if (addr >= 0x2000 && addr <= 0x3EFF) {
		addr &= 0x0FFF;
		if (cart->mirroring == Cartridge::Mirroring::VERTICAL) {
			if (addr >= 0x0000 && addr <= 0x03FF)
				nametables[0][addr & 0x03FF] = data;
			if (addr >= 0x0400 && addr <= 0x07FF)
				nametables[1][addr & 0x03FF] = data;
			if (addr >= 0x0800 && addr <= 0x0BFF)
				nametables[0][addr & 0x03FF] = data;
			if (addr >= 0x0C00 && addr <= 0x0EFF)
				nametables[1][addr & 0x03FF] = data;
		}
		else if (cart->mirroring == Cartridge::Mirroring::HORIZONTAL) {
			if (addr >= 0x0000 && addr <= 0x03FF)
				nametables[0][addr & 0x03FF] = data;
			if (addr >= 0x0400 && addr <= 0x07FF)
				nametables[0][addr & 0x03FF] = data;
			if (addr >= 0x0800 && addr <= 0x0BFF)
				nametables[1][addr & 0x03FF] = data;
			if (addr >= 0x0C00 && addr <= 0x0EFF)
				nametables[1][addr & 0x03FF] = data;
		}

	}
	else if (addr >= 0x3F00 && addr <= 0x3FFF) {
		addr &= 0x001F;
		if (addr == 0x0010) addr = 0x0000;
		if (addr == 0x0014) addr = 0x0004;
		if (addr == 0x0018) addr = 0x0008;
		if (addr == 0x001C) addr = 0x000C;

		palettes[addr] = data;
	}
}
