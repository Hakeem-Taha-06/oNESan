#include "Emulator.h"

Emulator::Emulator() {
	init();
}

Emulator::~Emulator() {
	
}

void Emulator::init(){
	// hardcoded reset vector
	bus.cpuWrite(0xFFFC, 0x00);
	bus.cpuWrite(0xFFFD, 0x04);

	bus.cpu.reset();
}

void Emulator::load(const std::string& path) {
	std::ifstream file( path, std::ios::binary | std::ios::ate);
	if (!file)
		throw std::runtime_error("Couldn't read file: "+path);

	size_t size = file.tellg();
	file.seekg(0, std::ios::beg);

	std::vector<uint8_t> buffer(size);
	if (!file.read(reinterpret_cast<char*>(buffer.data()), size))
		throw std::runtime_error("Failed to read file: " + path);
		
	for (int i = 0; i < size; ++i)
		// should start from 0x0000 but the klaus test rom always assembles with a 10 byte offset for some reason, this is temporary anyways and will be replaced 
		bus.cpuWrite(0x000a + (uint16_t)i, buffer[i]); 

	init();
}

void Emulator::clock() {
	bus.cpu.clock();
}
