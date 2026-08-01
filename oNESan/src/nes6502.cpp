#include "nes6502.h"
#include "Bus.h"

nes6502::nes6502() {

	//lookup table by javidx9 containing all the legal instructions on the 6502
	using a = nes6502;
	lookup =
	{
		{ "BRK", &a::BRK, &a::IMM, 7 },{ "ORA", &a::ORA, &a::IZX, 6 },{ "???", &a::XXX, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 8 },{ "???", &a::NOP, &a::IMP, 3 },{ "ORA", &a::ORA, &a::ZP0, 3 },{ "ASL", &a::ASL, &a::ZP0, 5 },{ "???", &a::XXX, &a::IMP, 5 },{ "PHP", &a::PHP, &a::IMP, 3 },{ "ORA", &a::ORA, &a::IMM, 2 },{ "ASL", &a::ASL, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 2 },{ "???", &a::NOP, &a::IMP, 4 },{ "ORA", &a::ORA, &a::ABS, 4 },{ "ASL", &a::ASL, &a::ABS, 6 },{ "???", &a::XXX, &a::IMP, 6 },
		{ "BPL", &a::BPL, &a::REL, 2 },{ "ORA", &a::ORA, &a::IZY, 5 },{ "???", &a::XXX, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 8 },{ "???", &a::NOP, &a::IMP, 4 },{ "ORA", &a::ORA, &a::ZPX, 4 },{ "ASL", &a::ASL, &a::ZPX, 6 },{ "???", &a::XXX, &a::IMP, 6 },{ "CLC", &a::CLC, &a::IMP, 2 },{ "ORA", &a::ORA, &a::ABY, 4 },{ "???", &a::NOP, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 7 },{ "???", &a::NOP, &a::IMP, 4 },{ "ORA", &a::ORA, &a::ABX, 4 },{ "ASL", &a::ASL, &a::ABX, 7 },{ "???", &a::XXX, &a::IMP, 7 },
		{ "JSR", &a::JSR, &a::ABS, 6 },{ "AND", &a::AND, &a::IZX, 6 },{ "???", &a::XXX, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 8 },{ "BIT", &a::BIT, &a::ZP0, 3 },{ "AND", &a::AND, &a::ZP0, 3 },{ "ROL", &a::ROL, &a::ZP0, 5 },{ "???", &a::XXX, &a::IMP, 5 },{ "PLP", &a::PLP, &a::IMP, 4 },{ "AND", &a::AND, &a::IMM, 2 },{ "ROL", &a::ROL, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 2 },{ "BIT", &a::BIT, &a::ABS, 4 },{ "AND", &a::AND, &a::ABS, 4 },{ "ROL", &a::ROL, &a::ABS, 6 },{ "???", &a::XXX, &a::IMP, 6 },
		{ "BMI", &a::BMI, &a::REL, 2 },{ "AND", &a::AND, &a::IZY, 5 },{ "???", &a::XXX, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 8 },{ "???", &a::NOP, &a::IMP, 4 },{ "AND", &a::AND, &a::ZPX, 4 },{ "ROL", &a::ROL, &a::ZPX, 6 },{ "???", &a::XXX, &a::IMP, 6 },{ "SEC", &a::SEC, &a::IMP, 2 },{ "AND", &a::AND, &a::ABY, 4 },{ "???", &a::NOP, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 7 },{ "???", &a::NOP, &a::IMP, 4 },{ "AND", &a::AND, &a::ABX, 4 },{ "ROL", &a::ROL, &a::ABX, 7 },{ "???", &a::XXX, &a::IMP, 7 },
		{ "RTI", &a::RTI, &a::IMP, 6 },{ "EOR", &a::EOR, &a::IZX, 6 },{ "???", &a::XXX, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 8 },{ "???", &a::NOP, &a::IMP, 3 },{ "EOR", &a::EOR, &a::ZP0, 3 },{ "LSR", &a::LSR, &a::ZP0, 5 },{ "???", &a::XXX, &a::IMP, 5 },{ "PHA", &a::PHA, &a::IMP, 3 },{ "EOR", &a::EOR, &a::IMM, 2 },{ "LSR", &a::LSR, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 2 },{ "JMP", &a::JMP, &a::ABS, 3 },{ "EOR", &a::EOR, &a::ABS, 4 },{ "LSR", &a::LSR, &a::ABS, 6 },{ "???", &a::XXX, &a::IMP, 6 },
		{ "BVC", &a::BVC, &a::REL, 2 },{ "EOR", &a::EOR, &a::IZY, 5 },{ "???", &a::XXX, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 8 },{ "???", &a::NOP, &a::IMP, 4 },{ "EOR", &a::EOR, &a::ZPX, 4 },{ "LSR", &a::LSR, &a::ZPX, 6 },{ "???", &a::XXX, &a::IMP, 6 },{ "CLI", &a::CLI, &a::IMP, 2 },{ "EOR", &a::EOR, &a::ABY, 4 },{ "???", &a::NOP, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 7 },{ "???", &a::NOP, &a::IMP, 4 },{ "EOR", &a::EOR, &a::ABX, 4 },{ "LSR", &a::LSR, &a::ABX, 7 },{ "???", &a::XXX, &a::IMP, 7 },
		{ "RTS", &a::RTS, &a::IMP, 6 },{ "ADC", &a::ADC, &a::IZX, 6 },{ "???", &a::XXX, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 8 },{ "???", &a::NOP, &a::IMP, 3 },{ "ADC", &a::ADC, &a::ZP0, 3 },{ "ROR", &a::ROR, &a::ZP0, 5 },{ "???", &a::XXX, &a::IMP, 5 },{ "PLA", &a::PLA, &a::IMP, 4 },{ "ADC", &a::ADC, &a::IMM, 2 },{ "ROR", &a::ROR, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 2 },{ "JMP", &a::JMP, &a::IND, 5 },{ "ADC", &a::ADC, &a::ABS, 4 },{ "ROR", &a::ROR, &a::ABS, 6 },{ "???", &a::XXX, &a::IMP, 6 },
		{ "BVS", &a::BVS, &a::REL, 2 },{ "ADC", &a::ADC, &a::IZY, 5 },{ "???", &a::XXX, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 8 },{ "???", &a::NOP, &a::IMP, 4 },{ "ADC", &a::ADC, &a::ZPX, 4 },{ "ROR", &a::ROR, &a::ZPX, 6 },{ "???", &a::XXX, &a::IMP, 6 },{ "SEI", &a::SEI, &a::IMP, 2 },{ "ADC", &a::ADC, &a::ABY, 4 },{ "???", &a::NOP, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 7 },{ "???", &a::NOP, &a::IMP, 4 },{ "ADC", &a::ADC, &a::ABX, 4 },{ "ROR", &a::ROR, &a::ABX, 7 },{ "???", &a::XXX, &a::IMP, 7 },
		{ "???", &a::NOP, &a::IMP, 2 },{ "STA", &a::STA, &a::IZX, 6 },{ "???", &a::NOP, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 6 },{ "STY", &a::STY, &a::ZP0, 3 },{ "STA", &a::STA, &a::ZP0, 3 },{ "STX", &a::STX, &a::ZP0, 3 },{ "???", &a::XXX, &a::IMP, 3 },{ "DEY", &a::DEY, &a::IMP, 2 },{ "???", &a::NOP, &a::IMP, 2 },{ "TXA", &a::TXA, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 2 },{ "STY", &a::STY, &a::ABS, 4 },{ "STA", &a::STA, &a::ABS, 4 },{ "STX", &a::STX, &a::ABS, 4 },{ "???", &a::XXX, &a::IMP, 4 },
		{ "BCC", &a::BCC, &a::REL, 2 },{ "STA", &a::STA, &a::IZY, 6 },{ "???", &a::XXX, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 6 },{ "STY", &a::STY, &a::ZPX, 4 },{ "STA", &a::STA, &a::ZPX, 4 },{ "STX", &a::STX, &a::ZPY, 4 },{ "???", &a::XXX, &a::IMP, 4 },{ "TYA", &a::TYA, &a::IMP, 2 },{ "STA", &a::STA, &a::ABY, 5 },{ "TXS", &a::TXS, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 5 },{ "???", &a::NOP, &a::IMP, 5 },{ "STA", &a::STA, &a::ABX, 5 },{ "???", &a::XXX, &a::IMP, 5 },{ "???", &a::XXX, &a::IMP, 5 },
		{ "LDY", &a::LDY, &a::IMM, 2 },{ "LDA", &a::LDA, &a::IZX, 6 },{ "LDX", &a::LDX, &a::IMM, 2 },{ "???", &a::XXX, &a::IMP, 6 },{ "LDY", &a::LDY, &a::ZP0, 3 },{ "LDA", &a::LDA, &a::ZP0, 3 },{ "LDX", &a::LDX, &a::ZP0, 3 },{ "???", &a::XXX, &a::IMP, 3 },{ "TAY", &a::TAY, &a::IMP, 2 },{ "LDA", &a::LDA, &a::IMM, 2 },{ "TAX", &a::TAX, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 2 },{ "LDY", &a::LDY, &a::ABS, 4 },{ "LDA", &a::LDA, &a::ABS, 4 },{ "LDX", &a::LDX, &a::ABS, 4 },{ "???", &a::XXX, &a::IMP, 4 },
		{ "BCS", &a::BCS, &a::REL, 2 },{ "LDA", &a::LDA, &a::IZY, 5 },{ "???", &a::XXX, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 5 },{ "LDY", &a::LDY, &a::ZPX, 4 },{ "LDA", &a::LDA, &a::ZPX, 4 },{ "LDX", &a::LDX, &a::ZPY, 4 },{ "???", &a::XXX, &a::IMP, 4 },{ "CLV", &a::CLV, &a::IMP, 2 },{ "LDA", &a::LDA, &a::ABY, 4 },{ "TSX", &a::TSX, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 4 },{ "LDY", &a::LDY, &a::ABX, 4 },{ "LDA", &a::LDA, &a::ABX, 4 },{ "LDX", &a::LDX, &a::ABY, 4 },{ "???", &a::XXX, &a::IMP, 4 },
		{ "CPY", &a::CPY, &a::IMM, 2 },{ "CMP", &a::CMP, &a::IZX, 6 },{ "???", &a::NOP, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 8 },{ "CPY", &a::CPY, &a::ZP0, 3 },{ "CMP", &a::CMP, &a::ZP0, 3 },{ "DEC", &a::DEC, &a::ZP0, 5 },{ "???", &a::XXX, &a::IMP, 5 },{ "INY", &a::INY, &a::IMP, 2 },{ "CMP", &a::CMP, &a::IMM, 2 },{ "DEX", &a::DEX, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 2 },{ "CPY", &a::CPY, &a::ABS, 4 },{ "CMP", &a::CMP, &a::ABS, 4 },{ "DEC", &a::DEC, &a::ABS, 6 },{ "???", &a::XXX, &a::IMP, 6 },
		{ "BNE", &a::BNE, &a::REL, 2 },{ "CMP", &a::CMP, &a::IZY, 5 },{ "???", &a::XXX, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 8 },{ "???", &a::NOP, &a::IMP, 4 },{ "CMP", &a::CMP, &a::ZPX, 4 },{ "DEC", &a::DEC, &a::ZPX, 6 },{ "???", &a::XXX, &a::IMP, 6 },{ "CLD", &a::CLD, &a::IMP, 2 },{ "CMP", &a::CMP, &a::ABY, 4 },{ "NOP", &a::NOP, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 7 },{ "???", &a::NOP, &a::IMP, 4 },{ "CMP", &a::CMP, &a::ABX, 4 },{ "DEC", &a::DEC, &a::ABX, 7 },{ "???", &a::XXX, &a::IMP, 7 },
		{ "CPX", &a::CPX, &a::IMM, 2 },{ "SBC", &a::SBC, &a::IZX, 6 },{ "???", &a::NOP, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 8 },{ "CPX", &a::CPX, &a::ZP0, 3 },{ "SBC", &a::SBC, &a::ZP0, 3 },{ "INC", &a::INC, &a::ZP0, 5 },{ "???", &a::XXX, &a::IMP, 5 },{ "INX", &a::INX, &a::IMP, 2 },{ "SBC", &a::SBC, &a::IMM, 2 },{ "NOP", &a::NOP, &a::IMP, 2 },{ "???", &a::SBC, &a::IMP, 2 },{ "CPX", &a::CPX, &a::ABS, 4 },{ "SBC", &a::SBC, &a::ABS, 4 },{ "INC", &a::INC, &a::ABS, 6 },{ "???", &a::XXX, &a::IMP, 6 },
		{ "BEQ", &a::BEQ, &a::REL, 2 },{ "SBC", &a::SBC, &a::IZY, 5 },{ "???", &a::XXX, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 8 },{ "???", &a::NOP, &a::IMP, 4 },{ "SBC", &a::SBC, &a::ZPX, 4 },{ "INC", &a::INC, &a::ZPX, 6 },{ "???", &a::XXX, &a::IMP, 6 },{ "SED", &a::SED, &a::IMP, 2 },{ "SBC", &a::SBC, &a::ABY, 4 },{ "NOP", &a::NOP, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 7 },{ "???", &a::NOP, &a::IMP, 4 },{ "SBC", &a::SBC, &a::ABX, 4 },{ "INC", &a::INC, &a::ABX, 7 },{ "???", &a::XXX, &a::IMP, 7 },
	};
}

nes6502::~nes6502() {

}

void nes6502::connectBus(Bus* b) {
	bus = b;
}

uint8_t nes6502::read(uint16_t address) {
	return bus->read(address);
}
void nes6502::write(uint16_t address, uint8_t data) {
	bus->write(address, data);
}

void nes6502::reset() {
	// Set program counter to reset vector stored at 0xFFFC/D
	uint8_t lo = read(0xFFFC);
	uint8_t hi = read(0xFFFD);
	pc = (hi << 8) | lo;

	stkp = 0xFD;
	acc = 0x00;
	xreg = 0x00;
	yreg = 0x00;
	stat = 0x01;

	cycles = 0x00;
}

void nes6502::clock() {
	
	if (cycles == 0) {
		//read the data at the program counter
		opcode = read(pc);
		pc++;

		cycles += lookup[opcode].cycles;

		//run the instruction
		uint8_t additionalCycle1 = (this->*lookup[opcode].addrmode)();
		uint8_t additionalCycle2 = (this->*lookup[opcode].operate)();

		cycles += (additionalCycle1 & additionalCycle2);
	}

	cycles--;
}

uint8_t nes6502::fetch(){
	if (lookup[opcode].addrmode != &nes6502::IMP)
		fetched = read(addr_abs);
	return fetched;
}

uint8_t nes6502::getFlag(FLAGS6502 f) {
	return (stat & f) > 0 ? 1 : 0;
}

void nes6502::setFlag(FLAGS6502 f, bool v) {
	if (v)
		stat |= f;
	else
		stat &= ~f;
}

// ---------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Addressing modes ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// ---------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------
// Implicit: no data needed, but some instructions operate on the value in the accumulator
uint8_t nes6502::IMP() {
	fetched = acc; // set the fetched data as the data in the accumulator
	return 0;
}

// Immediate: the next byte represents the target address 
uint8_t nes6502::IMM() {
	addr_abs = pc; // set the pc as the address then increment it
	pc++;
	return 0;
}

// Zero Page: the next byte represents the offset in the zero page, set the address to the result
uint8_t nes6502::ZP0() {
	uint8_t zeroPageOffset = read(pc);    // first we read the data at the program counter 
	pc++;                                 // increment the program counter
	addr_abs = zeroPageOffset & 0x00FF;   // the read data represents the offset into the zero page, set the result as the address
	return 0;
}

// Zero Page X: The address to be accessed by an instruction using indexed zero page addressing is calculated by taking the 
//             8 bit zero page address from the instruction and adding the current value of the X register to it
uint8_t nes6502::ZPX() {
	uint8_t zeroPageOffset = read(pc);             // read the data at the program counter
	pc++;                                          // increment the program counter
	addr_abs = ((zeroPageOffset + xreg) & 0x00FF); // the address is the LBO of the offset into the zero page + the contents of the x register
	return 0;
}

// Zero Page X: The address to be accessed by an instruction using indexed zero page addressing is calculated by taking the 
//             8 bit zero page address from the instruction and adding the current value of the Y register to it
uint8_t nes6502::ZPY() {
	uint8_t zeroPageOffset = read(pc);             // read the data at the program counter
	pc++;                                          // increment the program counter
	addr_abs = ((zeroPageOffset + yreg) & 0x00FF); // the address is the LBO of the offset into the zero page + the contents of the y register
	return 0;
}

// Absolute: the next two bytes represent the target location
uint8_t nes6502::ABS() {
	//assumes little endian order of low byte followed by high byte
	uint16_t lo = read(pc); // read the low byte of the address and increment pc
	pc++;
	uint16_t hi = read(pc); // read the high byte of the address and increment pc
	pc++;

	addr_abs = (hi << 8) | lo; // HHHH HHHH LLLL LLLL
	return 0;
}

uint8_t nes6502::ABX() { 
	//assumes little endian order of low byte followed by high byte
	uint16_t lo = read(pc); // read the low byte of the address and increment pc
	pc++;
	uint16_t hi = read(pc); // read the high byte of the address and increment pc
	pc++;

	addr_abs = (hi << 8) | lo; // HHHH HHHH LLLL LLLL
	addr_abs += xreg;

	// If adding xreg results in moving to another page then add another cycle for the instruction
	// this can be checked by checking for the high byte of the address and comparing it to the high byte before adding xreg
	if ((addr_abs & 0xFF00) != (hi << 8))
		return 1;

	return 0; 
}

uint8_t nes6502::ABY() {
	//assumes little endian order of low byte followed by high byte
	uint16_t lo = read(pc); // read the low byte of the address and increment pc
	pc++;
	uint16_t hi = read(pc); // read the high byte of the address and increment pc
	pc++;

	addr_abs = (hi << 8) | lo; // HHHH HHHH LLLL LLLL
	addr_abs += yreg;

	// If adding yreg results in moving to another page then add another cycle for the instruction
	// this can be checked by checking for the high byte of the address and comparing it to the high byte before adding yreg
	if ((addr_abs & 0xFF00) != (hi << 8))
		return 1;

	return 0;
}

// Indirect: the next two byte represent the memory location holding the value of the target location (pointers)
uint8_t nes6502::IND() { 

	uint16_t ptr_lo = read(pc);
	pc++;
	uint16_t ptr_hi = read(pc);
	pc++;

	uint16_t ptr = (ptr_hi << 8) | ptr_lo;

	if (ptr_lo == 0x00FF)  // Simulate page boundary hardware bug (the high byte failing to increment at the end of a page)
		addr_abs = (read(ptr & 0xFF00) << 8) | read(ptr);
	else 
		// Normal behaviour
		addr_abs = (read(ptr + 1) << 8) | read(ptr);
	
	return 0; 
}

// Indexed Indirect: the instruction specifies a zero page location and then the xreg is added to it,
//                   the resulting value represents the address where the target location value lives
uint8_t nes6502::IZX() {
	// the first operand represents the zero page index
	uint16_t zp = read(pc);
	pc++;

	// add the xreg to it and wrap around, this value represents the address of the target location
	uint16_t ptr = (zp + xreg) & 0x00FF;

	// read the two bytes at the location specified, this is the target location
	uint16_t lo = read(ptr);
	uint16_t hi = read(ptr + 1);

	addr_abs = (hi << 8) | lo;

	return 0;
}

// Indirect Indexed: the instruction specifies a zero page location which represents where 
//                   the unmodified target location lives, the yreg is then added to it, 
//                   the resulting value is the target location.
//                   if the addition of the yreg results in changing pages, add 1 extra cycle
uint8_t nes6502::IZY() {
	uint16_t zp = read(pc);
	pc++;

	uint16_t lo = read(zp & 0x00FF);      // read two bytes for the initial target location  
	uint16_t hi = read((zp + 1) & 0x00FF);

	uint16_t ptr = (hi << 8) | lo;
	addr_abs = ptr + yreg;                // add the yreg to get the final target location 

	// if this results in incrementing the page, add an extra cycle to the instruction
	if ((addr_abs & 0xFF00)!= hi << 8) 
		return 1;
	else 
		return 0;
}

// Relative: specifies a signed offset from -128 to 127 from the current program counter
uint8_t nes6502::REL() { 
	addr_rel = read(pc);
	pc++;
	if (addr_rel & 0x80)     // if the value is negative, set its high byte to all 1's 
		addr_rel |= 0xFF00;
	return 0;
}

// ---------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Operations ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// ---------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------
// Load Accumulator: Loads a byte of memory into the accumulator setting the zero and negative flags as appropriate
uint8_t nes6502::LDA() {
	acc = fetch();                  // reads a byte of memory into the accumulator
	setFlag(Z, acc == 0);           // set the zero flag if the accumulator is zero
	setFlag(O, (acc & (1<<7)) > 0); // set the overflow flag if the MSB is high
	return 0;
}

// Load X: Loads a byte of memory into the X register setting the zero and negative flags as appropriate
uint8_t nes6502::LDX() {
	xreg = fetch();                    // reads a byte of memory into the x register
	setFlag(Z, xreg == 0);             // set the zero flag if the x register is zero
	setFlag(O, (xreg & (1 << 7)) > 0); // set the overflow flag if the MSB is high
	return 0;
}

// Load Y: Loads a byte of memory into the Y register setting the zero and negative flags as appropriate
uint8_t nes6502::LDY() {
	yreg = fetch();                    // reads a byte of memory into the y register
	setFlag(Z, yreg == 0);             // set the zero flag if the y register is zero
	setFlag(O, (yreg & (1 << 7)) > 0); // set the overflow flag if the MSB is high
	return 0;
}

// Store Accumulator: stores accumulator contents into a memory address
uint8_t nes6502::STA() {
	write(addr_abs, acc);
	return 0;
}

// Store X: stores xreg contents into a memory address
uint8_t nes6502::STX() {
	write(addr_abs, xreg);
	return 0;
}

// Store Y: stores yreg contents into a memory address
uint8_t nes6502::STY() {
	write(addr_abs, yreg);
	return 0;
}

// Transfer Accumulator to X: transfers the contents of the accumulator to the xreg
uint8_t nes6502::TAX() {
	xreg = acc;
	setFlag(Z, xreg == 0);             // set the zero flag if the x register is zero
	setFlag(O, (xreg & (1 << 7)) > 0); // set the overflow flag if the MSB is high
	return 0;
}

// Transfer Accumulator to Y: transfers the contents of the accumulator to the yreg
uint8_t nes6502::TAY() {
	yreg = acc;
	setFlag(Z, yreg == 0);             // set the zero flag if the y register is zero
	setFlag(O, (yreg & (1 << 7)) > 0); // set the overflow flag if the MSB is high
	return 0;
}

// Transfer X to Accumulator: transfers the contents of the xreg to the accumulator
uint8_t nes6502::TXA() {
	acc = xreg;
	setFlag(Z, acc == 0);             // set the zero flag if the accumulator is zero
	setFlag(O, (acc & (1 << 7)) > 0); // set the overflow flag if the MSB is high
	return 0;
}

// Transfer Y to Accumulator: transfers the contents of the yreg to the accumulator
uint8_t nes6502::TYA() {
	acc = yreg;
	setFlag(Z, acc == 0);             // set the zero flag if the accumulator is zero
	setFlag(O, (acc & (1 << 7)) > 0); // set the overflow flag if the MSB is high
	return 0;
}

// Transfer Stack Pointer to X: transfers the contents of the stack pointer to the xreg
uint8_t nes6502::TSX() {
	xreg = stkp;
	setFlag(Z, xreg == 0);
	setFlag(O, (xreg & (1 << 7)) > 0);
	return 0;
}

// Transfer X to Stack Pointer: transfers the contents of the accumulator to the xreg
uint8_t nes6502::TXS() {
	stkp = xreg;
	return 0;
}

// Jump: Sets the program counter to the address specified by the operand
uint8_t nes6502::JMP() { 
	pc = addr_abs; // set the program counter as the address
	return 0; 
}

// to be implemented
uint8_t nes6502::ADC() { return 0; } uint8_t nes6502::AND() { return 0; } uint8_t nes6502::ASL() { return 0; } uint8_t nes6502::BCC() { return 0; }
uint8_t nes6502::BCS() { return 0; } uint8_t nes6502::BEQ() { return 0; } uint8_t nes6502::BIT() { return 0; } uint8_t nes6502::BMI() { return 0; }
uint8_t nes6502::BNE() { return 0; } uint8_t nes6502::BPL() { return 0; } uint8_t nes6502::BRK() { return 0; } uint8_t nes6502::BVC() { return 0; }
uint8_t nes6502::BVS() { return 0; } uint8_t nes6502::CLC() { return 0; } uint8_t nes6502::CLD() { return 0; } uint8_t nes6502::CLI() { return 0; }
uint8_t nes6502::CLV() { return 0; } uint8_t nes6502::CMP() { return 0; } uint8_t nes6502::CPX() { return 0; } uint8_t nes6502::CPY() { return 0; }
uint8_t nes6502::DEC() { return 0; } uint8_t nes6502::DEX() { return 0; } uint8_t nes6502::DEY() { return 0; } uint8_t nes6502::EOR() { return 0; }
uint8_t nes6502::INC() { return 0; } uint8_t nes6502::INX() { return 0; } uint8_t nes6502::INY() { return 0; } uint8_t nes6502::JSR() { return 0; } 
uint8_t nes6502::LSR() { return 0; } uint8_t nes6502::NOP() { return 0; } uint8_t nes6502::ORA() { return 0; } uint8_t nes6502::PHA() { return 0; }
uint8_t nes6502::PHP() { return 0; } uint8_t nes6502::PLA() { return 0; } uint8_t nes6502::PLP() { return 0; } uint8_t nes6502::ROL() { return 0; }
uint8_t nes6502::ROR() { return 0; } uint8_t nes6502::RTI() { return 0; } uint8_t nes6502::RTS() { return 0; } uint8_t nes6502::SBC() { return 0; }
uint8_t nes6502::SEC() { return 0; } uint8_t nes6502::SED() { return 0; } uint8_t nes6502::SEI() { return 0; } 

uint8_t nes6502::XXX() { return 0; }