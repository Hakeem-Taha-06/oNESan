#include "Bus.h"

// ----------------------------- TESTBUS
TestBus::TestBus() {
	cpu.connectBus(this);
	ram.fill(0x00);
}

TestBus::~TestBus() {

}

uint8_t TestBus::cpuRead(uint16_t address) {
	if(address >= 0x0000 && address <= 0xFFFF)
		return ram[address];

	return 0x00;  
}

void TestBus::cpuWrite(uint16_t address, uint8_t data) {
	if (address >= 0x0000 && address <= 0xFFFF)
		ram[address] = data;
}

void TestBus::clock() {
	cpu.clock();
}

void TestBus::reset() {
	cpu.reset();
}

// ----------------------------- NESBUS
NesBus::NesBus() {
	cpu.connectBus(this);
}

NesBus::~NesBus() {

}

void NesBus::insertCartridge(const std::shared_ptr<Cartridge>& cartridge) {
	cart = cartridge;
	ppu.insertCartridge(cart);
}

uint8_t NesBus::cpuRead(uint16_t addr) {
	uint8_t data = 0x00;
	if (addr >= 0x0000 && addr <= 0x1FFF) { // cpu ram
		data = cpuRam[addr & 0x07FF];
	}
	else if (addr >= 0x2000 && addr <= 0x3FFF) { // ppu registers
		data = ppu.cpuRead(addr & 0x0007);
	}
	else if (addr >= 0x4020 && addr <= 0xFFFF) { // cartridge PRG-ROM
		data = cart->cpuRead(addr);
	}
	return data;
}

void NesBus::cpuWrite(uint16_t addr, uint8_t data) {
	if (addr >= 0x0000 && addr <= 0x1FFF) { // cpu ram
		cpuRam[addr & 0x07FF] = data;
	}
	else if (addr >= 0x2000 && addr <= 0x3FFF) { // ppu registers
		ppu.cpuWrite(addr & 0x0007, data);
	}
	else if (addr >= 0x4020 && addr <= 0xFFFF) { // cartridge PRG-ROM
		cart->cpuWrite(addr, data);
	}
}

void NesBus::clock() {
	ppu.clock();

	// CPU is 3 times slower than the ppu (NTSC)
	if (numClocks % 3 == 0) {
		cpu.clock();
	}

	numClocks++;
}

void NesBus::reset() {
	cpu.reset();
}