#include "Cartridge.h"

Cartridge::Cartridge(std::string path) {
	std::ifstream file;
	file.open(path, std::ios::binary);
}


uint8_t Cartridge::cpuRead(uint16_t addr) {
	return 0x00;
}
void Cartridge::cpuWrite(uint16_t addr, uint8_t data) {

}