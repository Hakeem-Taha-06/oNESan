#include "Bus.h"

TestBus::TestBus() {
	cpu.connectBus(this);

	for (auto& i : ram) i = 0x00; //BRK
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


NesBus::NesBus() {
	//cpu.connectBus(this);
	cpuRam.fill(0x00);
}

NesBus::~NesBus() {

}

void NesBus::insertCartridge(const std::shared_ptr<Cartridge>& cartridge) {
	cart = cartridge;
}

uint8_t NesBus::cpuRead(uint16_t addr) {
	
	if (addr >= 0x0000 && addr <= 0x1FFF) { // cpu ram
		return cpuRam[addr ^ 0x07FF];
	}
	else if (addr >= 0x2000 && addr <= 0x3FFF) { // ppu registers
		return ppu.cpuRead(addr&0x0007);
	}
	else if (addr >= 0x4000 && addr <= 0xFFFF) { // cartridge PRG-ROM
		return cart->cpuRead(addr);
	}
}

void NesBus::cpuWrite(uint16_t addr, uint8_t data) {
	if (addr >= 0x0000 && addr <= 0x1FFF) { // cpu ram
		cpuRam[addr ^ 0x07FF] = data;
	}
	else if (addr >= 0x2000 && addr <= 0x3FFF) { // ppu registers
		ppu.cpuWrite(addr&0x0007, data);
	}
	else if (addr >= 0x4000 && addr <= 0xFFFF) { // cartridge PRG-ROM
		cart->cpuWrite(addr, data);
	}
}