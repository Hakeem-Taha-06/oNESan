#include "Emulator.h"

Emulator::Emulator() {
	init();
}

Emulator::~Emulator() {
	
}

void Emulator::init(){
	writeTestProgram();

	bus.cpu.reset();
}

void Emulator::writeTestProgram() {
	// --- memory setup: pointers & data for indirect addressing tests ---
	bus.write(0x0055, 0x00); // IZX pointer lo  -> points to 0x0400   (zp $50 + X=5 = $55)
	bus.write(0x0056, 0x04); // IZX pointer hi
	bus.write(0x0400, 0x77); // IZX target data

	bus.write(0x0060, 0x00); // IZY pointer lo  -> points to 0x0500   (zp $60)
	bus.write(0x0061, 0x05); // IZY pointer hi
	bus.write(0x0503, 0x88); // IZY target data (0x0500 + Y=3)

	bus.write(0x0300, 0x2E); // IND pointer lo  -> points to 0x022E (the loop instruction)
	bus.write(0x0301, 0x02); // IND pointer hi

	bus.write(0xFFFC, 0x00); // reset vector
	bus.write(0xFFFD, 0x02); //   -> 0x0200

	// - start - hardcoded test program
	bus.write(0x0200, 0xA9); // LDA #$F1          (IMM)
	bus.write(0x0201, 0xF1); //     ^
	bus.write(0x0202, 0xA2); // LDX #$05          (IMM)
	bus.write(0x0203, 0x05); //     ^
	bus.write(0x0204, 0xA0); // LDY #$03          (IMM)
	bus.write(0x0205, 0x03); //     ^

	bus.write(0x0206, 0x85); // STA $10           (ZP0)  mem[0x10] = F1
	bus.write(0x0207, 0x10); //     ^
	bus.write(0x0208, 0xA5); // LDA $10           (ZP0)  confirm A = F1
	bus.write(0x0209, 0x10); //     ^

	bus.write(0x020A, 0x95); // STA $20,X         (ZPX)  mem[0x25] = F1
	bus.write(0x020B, 0x20); //     ^

	bus.write(0x020C, 0x86); // STX $30           (ZP0)  mem[0x30] = 05
	bus.write(0x020D, 0x30); //     ^

	bus.write(0x020E, 0x96); // STX $40,Y         (ZPY)  mem[0x43] = 05
	bus.write(0x020F, 0x40); //     ^

	bus.write(0x0210, 0xA9); // LDA #$AA          (IMM)
	bus.write(0x0211, 0xAA); //     ^

	bus.write(0x0212, 0x8D); // STA $0600         (ABS)  mem[0x0600] = AA
	bus.write(0x0213, 0x00); //     ^ lo
	bus.write(0x0214, 0x06); //     ^ hi
	bus.write(0x0215, 0xAD); // LDA $0600         (ABS)  confirm A = AA
	bus.write(0x0216, 0x00); //     ^ lo
	bus.write(0x0217, 0x06); //     ^ hi

	bus.write(0x0218, 0x9D); // STA $0620,X       (ABX)  mem[0x0625] = AA
	bus.write(0x0219, 0x20); //     ^ lo
	bus.write(0x021A, 0x06); //     ^ hi

	bus.write(0x021B, 0x99); // STA $0630,Y       (ABY)  mem[0x0633] = AA
	bus.write(0x021C, 0x30); //     ^ lo
	bus.write(0x021D, 0x06); //     ^ hi
	bus.write(0x021E, 0xB9); // LDA $0630,Y       (ABY)  confirm A = AA
	bus.write(0x021F, 0x30); //     ^ lo
	bus.write(0x0220, 0x06); //     ^ hi

	bus.write(0x0221, 0xA1); // LDA ($50,X)       (IZX)  A = mem[0x0400] = 0x77
	bus.write(0x0222, 0x50); //     ^

	bus.write(0x0223, 0xB1); // LDA ($60),Y       (IZY)  A = mem[0x0503] = 0x88
	bus.write(0x0224, 0x60); //     ^

	bus.write(0x0225, 0xAA); // TAX               (IMP)  X = A = 0x88
	bus.write(0x0226, 0x8A); // TXA               (IMP)  A = X = 0x88
	bus.write(0x0227, 0xA8); // TAY               (IMP)  Y = A = 0x88
	bus.write(0x0228, 0x98); // TYA               (IMP)  A = Y = 0x88
	bus.write(0x0229, 0x9A); // TXS               (IMP)  stkp = X = 0x88
	bus.write(0x022A, 0xBA); // TSX               (IMP)  X = stkp = 0x88

	bus.write(0x022B, 0x6C); // JMP ($0300)       (IND)  pc = mem@0x0300/0x0301 = 0x022E
	bus.write(0x022C, 0x00); //     ^ lo
	bus.write(0x022D, 0x03); //     ^ hi

	bus.write(0x022E, 0x4C); // JMP $022E         (ABS)  infinite loop, marks clean end
	bus.write(0x022F, 0x2E); //     ^ lo
	bus.write(0x0230, 0x02); //     ^ hi
	// - end -   hardcoded test program

	bus.cpu.reset();
}

void Emulator::clock() {
	bus.cpu.clock();
}
