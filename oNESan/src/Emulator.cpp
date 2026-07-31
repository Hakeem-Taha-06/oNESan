#include "Emulator.h"

Emulator::Emulator() {
	init();
}

Emulator::~Emulator() {
	
}

void Emulator::init() {

	bus.write(0x0000, 0x21); // some data in the zero page

	bus.write(0xFFFC, 0x00); // reset vector
	bus.write(0xFFFD, 0x02); //       

	// - start - hardcoded test program
	bus.write(0x0200, 0xA9); //LDA #F1
	bus.write(0x0201, 0xF1); //     ^
	bus.write(0x0202, 0xA4); //LDY $00
	bus.write(0x0203, 0x00); //     ^
	// - end -   hardcoded test program\

	bus.cpu.reset();

}

void Emulator::clock() {
	bus.cpu.clock();
}

void Emulator::runTestProgram() {
	// LDA
	bus.cpu.clock();
	bus.cpu.clock();
	// LDY  
	bus.cpu.clock();
	bus.cpu.clock();
}