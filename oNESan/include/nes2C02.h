#pragma once

#include <cstdint>
#include <memory>

class Cartridge;

class nes2C02
{
public:
	nes2C02();

	void clock();

	uint8_t cpuRead(uint16_t addr);
	void cpuWrite(uint16_t addr, uint8_t data);

	// ppu bus
	uint8_t ppuRead(uint16_t addr);
	void ppuWrite(uint16_t addr, uint8_t data);

	void insertCartridge(const std::shared_ptr<Cartridge>& cartridge);

	uint32_t getColorFromPalette(uint8_t palette, uint8_t pixel);
	float* getPatternTable(uint8_t i, uint8_t palette);
	const float* getScreen();

	bool emitNmi = false;
private:
	static constexpr uint32_t NES_PALETTE[64] = {
		// Row 0x00 - 0x0F
		0x00545454, 0x00001E74, 0x00081090, 0x00300088,
		0x00440064, 0x005C0030, 0x00540400, 0x003C1800,
		0x00202A00, 0x00083A00, 0x00004000, 0x00003C00,
		0x0000323C, 0x00000000, 0x00000000, 0x00000000,

		// Row 0x10 - 0x1F
		0x00989698, 0x00084CC4, 0x003032EC, 0x005C1EE4,
		0x008814B0, 0x00A01464, 0x00982220, 0x00783C00,
		0x00545A00, 0x00287200, 0x00087C00, 0x00007628,
		0x00006678, 0x00000000, 0x00000000, 0x00000000,

		// Row 0x20 - 0x2F
		0x00ECECEC, 0x004C9AEC, 0x00787CEC, 0x00B062EC,
		0x00E454EC, 0x00EC58B4, 0x00EC6A64, 0x00D48820,
		0x00A0AA00, 0x0074C400, 0x004CD020, 0x0038CC6C,
		0x0038B4CC, 0x003C3C3C, 0x00000000, 0x00000000,

		// Row 0x30 - 0x3F
		0x00ECECEC, 0x00A8CCE4, 0x00BCBCE4, 0x00D4B2EC,
		0x00ECAEEC, 0x00ECAED4, 0x00ECB4B0, 0x00E4C490,
		0x00CCC078, 0x00B4DE78, 0x00A8E290, 0x0098E2B4,
		0x00A0D6E4, 0x00A0A2A0, 0x00000000, 0x00000000
	};

	std::shared_ptr<Cartridge> cart;

	// starts at $3F00
	// each pallete is 4 bytes long, each byte contains a 6 bit index
	// there are 4 palettes each for sprites and backgrounds
	// Background palettes at $3F0x
	// Sprite     palettes at $3F1x
	uint8_t palettes[32];
	uint8_t nametables[2][1024];

	// ---------- screen texture data to be read by the renderer
	
	// two screens for displaying the pattern tables, each containing 128*128 pixels with 3 channels (RGB) normalized for openGL use
	float patternScreen[2][128 * 128 * 3];
	float screen[256 * 240 * 3];

	int cycle = 0;
	int scanline = 0;

	// registers

	union PPUStatus{
		struct {
			uint8_t unused : 5;
			uint8_t spriteOverflow : 1;
			uint8_t spriteZeroHit : 1;
			uint8_t verticalBlank : 1;
		};
		uint8_t reg;
	} status;

	union PPUMAsk {
		struct {
			uint8_t grayscale : 1;
			uint8_t renderBGLeft : 1;
			uint8_t renderSpritesLeft : 1;
			uint8_t renderBG : 1;
			uint8_t renderSprites : 1;
			uint8_t enhanceRed : 1;
			uint8_t enhanceGreen : 1;
			uint8_t enhanceBlue : 1;
		};
		uint8_t reg;
	} mask;

	union PPUCTRL {
		struct {
			uint8_t nametableX : 1;
			uint8_t nametableY : 1;
			uint8_t incrementMode : 1;
			uint8_t patternSprite : 1;
			uint8_t patternBG : 1;
			uint8_t spriteSize : 1;
			uint8_t slaveMode : 1;
			uint8_t enableNmi : 1;
		};
		uint8_t reg;
	} control;

	uint8_t addressLatch = 0x00;
	uint8_t ppuDataBuffer = 0x00;

	union LoopyRegister {
		struct {
			uint16_t coarseX : 5;
			uint16_t coarseY : 5;
			uint16_t nametableX : 1;
			uint16_t nametableY : 1;
			uint16_t fineY : 3;
			uint16_t unused : 1;
		};
		uint16_t reg;
	};
	LoopyRegister vramAddr;
	LoopyRegister tramAddr;

	uint8_t fineX = 0x00;

	// Background Rendering
	uint8_t bgNextTileID     = 0x00;
	uint8_t bgNextTileAttrib = 0x00;
	uint8_t bgNextTileLsb    = 0x00;
	uint8_t bgNextTileMsb    = 0x00;

	uint16_t bgShifterPatternLo = 0x0000;
	uint16_t bgShifterPatternHi = 0x0000;
	uint16_t bgShifterAttribLo = 0x0000;
	uint16_t bgShifterAttribHi = 0x0000;
};

