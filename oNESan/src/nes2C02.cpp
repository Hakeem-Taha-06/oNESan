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
				vramAddr.fineY = tramAddr.fineY;
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

			if (mask.renderSprites && cycle >= 1 && cycle < 258) {
				for (int i = 0; i < spriteCount; ++i) {
					if (spriteScanline[i].x > 0) {
						spriteScanline[i].x--;
					}
					else {
						spriteShifterPatternLo[i] <<= 1;
						spriteShifterPatternHi[i] <<= 1;
					}
				}
			}
		};

	// visible scanlines
	if (scanline >= -1 && scanline < 240) {

		// odd frame cycle skip
		if (scanline == 0 && cycle == 0) {
			cycle = 1;
		}

		if (scanline == -1 && cycle == 1) { 
			status.verticalBlank = 0;

			status.spriteOverflow = 0;

			for (int i = 0; i < 8; ++i) {
				spriteShifterPatternLo[i] = 0;
				spriteShifterPatternHi[i] = 0;
			}
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
					bgNextTileLsb = ppuRead((control.patternBG << 12) + ((uint16_t)bgNextTileID << 4) + vramAddr.fineY);
					break;
				case 6:
					bgNextTileMsb = ppuRead((control.patternBG << 12) + ((uint16_t)bgNextTileID << 4) + vramAddr.fineY + 8);
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
			loadBackgroundShifters();
			transferAddressX();
		}

		if (cycle == 338 || cycle == 340) {
			bgNextTileID = ppuRead(0x2000 | vramAddr.reg & 0x0FFF);
		}

		if (scanline == -1 && cycle >= 280 && cycle < 305) {
			transferAddressY();
		}

		if (scanline == -1 && cycle >= 280 && cycle < 305) {
			transferAddressY();
		}

		// FOREGROUND RENDERING (not cycle accurate)
		if (cycle == 257 && scanline >= 0) {
			std::memset(spriteScanline, 0xFF, 8 * sizeof(OAMEntry));
			spriteCount = 0;

			for (uint8_t i = 0; i < 8; ++i) {
				spriteShifterPatternLo[i] = 0;
				spriteShifterPatternHi[i] = 0;
			}

			// for each sprite we check if the current scanline is contained within it
			uint8_t OAMidx = 0;
			while (OAMidx < 64 && spriteCount < 9) {
				int16_t diff = (int16_t)scanline - (int16_t)OAM[OAMidx].y;
				// sprite height could be 16 or 8 pixels long
				if (diff >= 0 && diff < (control.spriteSize ? 16 : 8)) {
					if (spriteCount < 8) {
						memcpy(&spriteScanline[spriteCount], &OAM[OAMidx], sizeof(OAMEntry));
						spriteCount++;
					}
				}
				OAMidx++;
			}
			status.spriteOverflow = (spriteCount > 8);
		}

		if (cycle == 340) {
			for (uint8_t i = 0; i < spriteCount; ++i) {
				uint8_t spritePatternBitsLo, spritePatternBitsHi;
				uint8_t spritePatternAddrLo, spritePatternAddrHi;

				if (control.spriteSize == 0) { // 8*8 sprite mode
					if (!(spriteScanline[i].attribute & 0x80)) { // not flipped vertically (normal
						spritePatternAddrLo =
							(control.patternSprite << 12)       // pattern table number
							| (spriteScanline[i].id << 4)       // which tile in the pattern table
							| (scanline - spriteScanline[i].y); // which row of the tile
					}
					else { // flipped vertically
						spritePatternAddrLo =
							(control.patternSprite << 12)             // pattern table number
							| (spriteScanline[i].id << 4)             // which tile in the pattern table
							| (7 - (scanline - spriteScanline[i].y)); // which row of the tile (inverted)
					}
				}
				else { // 8*16 sprite mode
					if (!(spriteScanline[i].attribute & 0x80)) { // not flipped vertically (normal
						if (scanline - spriteScanline[i].y < 8) { // top 8 rows 
							spritePatternAddrLo =
								((spriteScanline[i].id & 0x01) << 12)	     // pattern table number (implied by the bottom bit of the id for some reason)
								| ((spriteScanline[i].id & 0xFE) << 4)	     // which tile in the pattern table
								| ((scanline - spriteScanline[i].y) & 0x07); // which row in the tile
						}
						else { // bottom 8 rows
							spritePatternAddrLo =
								((spriteScanline[i].id & 0x01) << 12)	     // pattern table number (implied by the bottom bit of the id for some reason)
								| (((spriteScanline[i].id & 0xFE) + 1) << 4) // which tile in the pattern table
								| ((scanline - spriteScanline[i].y) & 0x07); // which row in the tile
						}
					}
					else { // flipped vertically
						if (scanline - spriteScanline[i].y < 8) { // top 8 rows 
							spritePatternAddrLo =
								((spriteScanline[i].id & 0x01) << 12)	         // pattern table number (implied by the bottom bit of the id for some reason)
								| (((spriteScanline[i].id & 0xFE) + 1) << 4)     // which tile in the pattern table
								| (7 - (scanline - spriteScanline[i].y) & 0x07); // which row in the tile (inverted)
						}
						else { // bottom 8 rows
							spritePatternAddrLo =
								((spriteScanline[i].id & 0x01) << 12)	         // pattern table number (implied by the bottom bit of the id for some reason)
								| ((spriteScanline[i].id & 0xFE) << 4)	         // which tile in the pattern table
								| (7 - (scanline - spriteScanline[i].y) & 0x07); // which row in the tile (inverted)
						}
					}
				}

			
				spritePatternAddrHi = spritePatternAddrLo + 8;

				spritePatternBitsLo = ppuRead(spritePatternAddrLo);
				spritePatternBitsHi = ppuRead(spritePatternAddrHi);

				if (spriteScanline[i].attribute & 0x40) { // horizontal flip
					spritePatternBitsLo = flipByte(spritePatternBitsLo);
					spritePatternBitsHi = flipByte(spritePatternBitsHi);
				}

				spriteShifterPatternLo[i] = spritePatternBitsLo;
				spriteShifterPatternHi[i] = spritePatternBitsHi;
			}
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
		uint8_t pixelLo = (bgShifterPatternLo & bitMux) > 0;
		uint8_t pixelHi = (bgShifterPatternHi & bitMux) > 0;
		bgPixel = (pixelHi << 1) | pixelLo;

		uint8_t palLo = (bgShifterAttribLo & bitMux) > 0;
		uint8_t palHi = (bgShifterAttribHi & bitMux) > 0;
		bgPalette = (palHi << 1) | palLo;
	}

	uint8_t fgPixel = 0x00;
	uint8_t fgPalette = 0x00;
	uint8_t fgPriority = 0x00;
	if (mask.renderSprites) {
		for (uint8_t i = 0; i < spriteCount; ++i) {
			if (spriteScanline[i].x == 0) {
				uint8_t fgPixelLo = (spriteShifterPatternLo[i] & 0x80) > 0;
				uint8_t fgPixelHi = (spriteShifterPatternHi[i] & 0x80) > 0;
				fgPixel = (fgPixelHi << 1) | fgPixelLo;

				fgPalette = (spriteScanline[i].attribute & 0x03) + 0x04;    // sprite palettes are the last four palettes
				fgPriority = (spriteScanline[i].attribute & 0x20) == 0; 

				// only the first non transparent pixel found gets drawn
				if (fgPixel != 0) {
					break;
				}
			}
		}
	}

	uint8_t pixel = 0x00;
	uint8_t palette = 0x00;

	if (bgPixel == 0 && fgPixel == 0) {
		pixel = 0x00;
		palette = 0x00;
	}
	else if(bgPixel > 0 && fgPixel == 0){
		pixel = bgPixel;
		palette = bgPalette;
	}
	else if (bgPixel == 0 && fgPixel > 0) {
		pixel = fgPixel;
		palette = fgPalette;
	}
	else if (bgPixel > 0 && fgPixel > 0) {
		if (fgPriority) {
			pixel = fgPixel;
			palette = fgPalette;
		}
		else {
			pixel = bgPixel;
			palette = bgPalette;
		}
	}

	// actual screen drawing
	if (scanline >= 0 && scanline < 240 && cycle >= 1 && cycle <= 256) {
		int idx = (scanline * 256 + cycle - 1) * 3;
		uint32_t color = getColorFromPalette(palette, pixel);
		screen[idx + 0] = (float)(color >> 16 & 0x0000FF) / 255;
		screen[idx + 1] = (float)(color >> 8  & 0x0000FF) / 255;
		screen[idx + 2] = (float)(color >> 0  & 0x0000FF) / 255;
	}

	cycle++;
	if (cycle >= 341) {
		cycle = 0;
		scanline++;
		if (scanline >= 261) {
			scanline = -1;
			frameComplete = true;
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
			data = oamArray[oamAddr];
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
			oamAddr = data;
			break;
		case 4:    // OAMDATA
			oamArray[oamAddr] = data;
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
				tramAddr.reg = (tramAddr.reg & 0x00FF) | (uint16_t)((data & 0x3F) << 8);
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

	if (cart->ppuRead(addr, data)) {

	}
	else if (addr >= 0x0000 && addr <= 0x1FFF) {
		data = patternTables[(addr & 0x1000)>>12][addr & 0x0FFF];
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
			if (addr >= 0x0C00 && addr <= 0x0FFF)
				data = nametables[1][addr & 0x03FF];
		}
		else if (cart->mirroring == Cartridge::Mirroring::HORIZONTAL) {
			if (addr >= 0x0000 && addr <= 0x03FF)
				data = nametables[0][addr & 0x03FF];
			if (addr >= 0x0400 && addr <= 0x07FF)
				data = nametables[0][addr & 0x03FF];
			if (addr >= 0x0800 && addr <= 0x0BFF)
				data = nametables[1][addr & 0x03FF];
			if (addr >= 0x0C00 && addr <= 0x0FFF)
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
	if (cart->ppuWrite(addr, data)) {

	}
	else if (addr >= 0x0000 && addr <= 0x1FFF) {
		patternTables[(addr & 0x1000) >> 12][addr & 0x0FFF] = data;
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
			if (addr >= 0x0C00 && addr <= 0x0FFF)
				nametables[1][addr & 0x03FF] = data;
		}
		else if (cart->mirroring == Cartridge::Mirroring::HORIZONTAL) {
			if (addr >= 0x0000 && addr <= 0x03FF)
				nametables[0][addr & 0x03FF] = data;
			if (addr >= 0x0400 && addr <= 0x07FF)
				nametables[0][addr & 0x03FF] = data;
			if (addr >= 0x0800 && addr <= 0x0BFF)
				nametables[1][addr & 0x03FF] = data;
			if (addr >= 0x0C00 && addr <= 0x0FFF)
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

void nes2C02::reset() {
	cycle = 0;
	scanline = 0;
	status.reg = 0x00;
	mask.reg = 0x00;
	control.reg = 0x00;
	addressLatch = 0x00;
	ppuDataBuffer = 0x00;
	vramAddr.reg = 0x00;
	tramAddr.reg = 0x00;
	fineX = 0x00;
	bgNextTileID = 0x00;
	bgNextTileAttrib = 0x00;
	bgNextTileLsb = 0x00;
	bgNextTileMsb = 0x00;
	bgShifterPatternLo = 0x0000;
	bgShifterPatternHi = 0x0000;
	bgShifterAttribLo = 0x0000;
	bgShifterAttribHi = 0x0000;
	std::memset(OAM, 0x00, sizeof(OAM));
}

// HELPER FUNCTIONS

// flips a byte
//uint8_t nes2C02::flipByte(uint8_t byte) {
//	uint8_t flipped = 0x00;
//	for (int i = 8; i > 0; --i) {
//		flipped = flipped | ((byte & 0x01) << i);
//		byte >>= 1;
//	}
//
//	return flipped;
//}

// flips a byte (Divide and Conquer algorithm aka O(1) hacky method)
uint8_t nes2C02::flipByte(uint8_t byte) {
	byte = (byte & 0xF0) >> 4 | (byte & 0x0F) << 4;
	byte = (byte & 0xCC) >> 2 | (byte & 0x33) << 2;
	byte = (byte & 0xAA) >> 1 | (byte & 0x55) << 1;
	return byte;
}
