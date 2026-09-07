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
	if (cart->cpuRead(addr, data)) {

	}
	else if(addr >= 0x0000 && addr <= 0x1FFF) { // cpu ram
		data = cpuRam[addr & 0x07FF];
	}
	else if (addr >= 0x2000 && addr <= 0x3FFF) { // ppu registers
		data = ppu.cpuRead(addr & 0x0007);
	}
	else if (addr == 0x4016) {
		data = joypad.read();
	}
	else if (addr == 0x4017) {
		data = 0x00;
	}
	
	return data;
}

void NesBus::cpuWrite(uint16_t addr, uint8_t data) {
	if (cart->cpuWrite(addr, data)) {

	}
	else if (addr >= 0x0000 && addr <= 0x1FFF) { // cpu ram
		cpuRam[addr & 0x07FF] = data;
	}
	else if (addr >= 0x2000 && addr <= 0x3FFF) { // ppu registers
		ppu.cpuWrite(addr & 0x0007, data);
	}
	else if (addr == 0x4014) {
		// start DMA, suspend cpu for 512 clock cycles
		dmaPage = data;
		dmaAddr = 0x00;
		dmaTransfer = true;
	}
	else if(addr == 0x4016){
		data &= 0x01;
		joypad.setLatch(data);
	}
}

void NesBus::clock() {
	ppu.clock();

	// CPU is 3 times slower than the ppu (NTSC)
	if (systemClockCounter % 3 == 0) {
		if (dmaTransfer) {
			// dma can only start on an odd cycle
			if (dmaDummy) {

				if (systemClockCounter % 2 == 1) {
					dmaDummy = false;
				}
			
			}
			else {
				// read data on even cycles
				if (systemClockCounter % 2 == 0) {
					dmaData = cpuRead(dmaPage << 8 | dmaAddr);
				}// write data on odd cycles
				else {
					ppu.oamArray[dmaAddr] = dmaData;
					dmaAddr++;

					// if the address wraps around, then we have written the full 255 bytes, so we exit DMA
					if (dmaAddr == 0x00) {
						dmaTransfer = false;
						dmaDummy = true;
					}
				}
			}
		}
		else {
			cpu.clock();
		}
	}

	if (ppu.emitNmi) {
		cpu.nmi();
		ppu.emitNmi = false;
	}

	systemClockCounter++;
}

void NesBus::reset() {
	cpu.reset();
	ppu.reset();
}