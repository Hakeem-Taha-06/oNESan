#include "Emulator.h"

Emulator::Emulator() {
	init();
}

Emulator::~Emulator() {
	
}

void Emulator::init() {

	// - start - hardcoded test program
	bus.write(0xFFFC, 0x4C); //JMP $0200   (instead of the regular reset vector, i just added a jump instruction)  
	bus.write(0xFFFD, 0x00); //       ^
	bus.write(0xFFFE, 0x02); //     ^
	bus.write(0x0200, 0xA9); //LDA #F1
	bus.write(0x0201, 0xF1); //     ^
	// - end -   hardcoded test program\

	bus.cpu.reset();

}

void Emulator::clock() {
	bus.cpu.clock();
}

void Emulator::runTestProgram() {
	// JMP -> should set pc to 0x0200 and modify no flags
	bus.cpu.clock();
	bus.cpu.clock();
	bus.cpu.clock();
	// LDA -> should load acc with 0xF1 and set O flag to 1 
	bus.cpu.clock();
	bus.cpu.clock();
}