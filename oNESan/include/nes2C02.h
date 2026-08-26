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

	uint32_t getColorFromPalette(int palette, int pixel);
	float* getPatternTable(int i, int palette);
private:
	static constexpr uint32_t NES_PALETTE[64] = {
		0x7C7C7C, 0x0000FC, 0x0000BC, 0x4428BC, 0x940084, 0xA80020, 0xA81000, 0x881400,
		0x503000, 0x007800, 0x006800, 0x005800, 0x004058, 0x000000, 0x000000, 0x000000,
		0xBCBCBC, 0x0078F8, 0x0058F8, 0x6844FC, 0xD800CC, 0xE40058, 0xF83800, 0xE45C10,
		0xAC7C00, 0x00B800, 0x00A800, 0x00A844, 0x008888, 0x000000, 0x000000, 0x000000,
		0xF8F8F8, 0x3CBCFC, 0x6888FC, 0x9878F8, 0xF878F8, 0xF85898, 0xF87858, 0xFCA044,
		0xF8B800, 0xB8F818, 0x58D854, 0x58F898, 0x00E8D8, 0x787878, 0x000000, 0x000000,
		0xFCFCFC, 0xA4E4FC, 0xB8B8F8, 0xD8B8F8, 0xF8B8F8, 0xF8A4C0, 0xF0D0B0, 0xFCE0A8,
		0xF8D878, 0xD8F878, 0xB8F8B8, 0xB8F8D8, 0x00FCFC, 0xF8D8F8, 0x000000, 0x000000
	};

	std::shared_ptr<Cartridge> cart;

	// starts at $3F00
	// each pallete is 4 bytes long, each byte contains a 6 bit index
	// there are 4 palettes each for sprites and backgrounds
	// Background palettes at $3F0x
	// Sprite     palettes at $3F1x
	uint8_t palettes[32]{0x05};
	uint8_t nametables[2][1024]{0x00};

	// ---------- screen texture data to be read by the renderer
	
	// two screens for displaying the pattern tables, each containing 128*128 pixels with 3 channels (RGB) normalized for openGL use
	float patternScreen[2][128 * 128 * 3]{0.0f};

	int cycle = 0;
	int scanline = 0;
};

