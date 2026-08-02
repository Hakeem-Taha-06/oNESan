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

	bus.write(0x22E, 0xA0);

	bus.write(0x022E, 0x4C); // JMP $022E         (ABS)  infinite loop, marks clean end
	bus.write(0x022F, 0x2E); //     ^ lo
	bus.write(0x0230, 0x02); //     ^ hi
	// - end -   hardcoded test program

	bus.cpu.reset();
}

void Emulator::writeTestProgram2() {
	bus.write(0x0095, 0x82); // pre-seeded data for the zero-page ASL test

	bus.write(0xFFFC, 0x00); // reset vector
	bus.write(0xFFFD, 0x02); //   -> 0x0200

	// - start - hardcoded test program

	// === BCS: C=1 -> should be TAKEN. Expect mem[$80] == 0xAA ===
	bus.write(0x0200, 0xA2); // LDX #$AA        baseline marker
	bus.write(0x0201, 0xAA); //     ^
	bus.write(0x0202, 0xA9); // LDA #$FF
	bus.write(0x0203, 0xFF); //     ^
	bus.write(0x0204, 0x69); // ADC #$01        0xFF+1 -> C=1
	bus.write(0x0205, 0x01); //     ^
	bus.write(0x0206, 0xB0); // BCS +2          C=1, should branch over the next instr
	bus.write(0x0207, 0x02); //     ^
	bus.write(0x0208, 0xA2); // LDX #$DE        only runs if BCS did NOT branch
	bus.write(0x0209, 0xDE); //     ^
	bus.write(0x020A, 0x86); // STX $80
	bus.write(0x020B, 0x80); //     ^

	// === BCC: C=1 -> should be NOT TAKEN. Expect mem[$81] == 0xDE ===
	bus.write(0x020C, 0xA2); // LDX #$AA
	bus.write(0x020D, 0xAA); //     ^
	bus.write(0x020E, 0xA9); // LDA #$FF
	bus.write(0x020F, 0xFF); //     ^
	bus.write(0x0210, 0x69); // ADC #$01        C=1 again
	bus.write(0x0211, 0x01); //     ^
	bus.write(0x0212, 0x90); // BCC +2          C=1, should fall through
	bus.write(0x0213, 0x02); //     ^
	bus.write(0x0214, 0xA2); // LDX #$DE        runs since branch not taken
	bus.write(0x0215, 0xDE); //     ^
	bus.write(0x0216, 0x86); // STX $81
	bus.write(0x0217, 0x81); //     ^

	// === BEQ: Z=1 -> should be TAKEN. Expect mem[$82] == 0xAA ===
	bus.write(0x0218, 0xA2); // LDX #$AA
	bus.write(0x0219, 0xAA); //     ^
	bus.write(0x021A, 0xA9); // LDA #$00        Z=1
	bus.write(0x021B, 0x00); //     ^
	bus.write(0x021C, 0xF0); // BEQ +2
	bus.write(0x021D, 0x02); //     ^
	bus.write(0x021E, 0xA2); // LDX #$DE
	bus.write(0x021F, 0xDE); //     ^
	bus.write(0x0220, 0x86); // STX $82
	bus.write(0x0221, 0x82); //     ^

	// === BNE: Z=1 -> should be NOT TAKEN. Expect mem[$83] == 0xDE ===
	bus.write(0x0222, 0xA2); // LDX #$AA
	bus.write(0x0223, 0xAA); //     ^
	bus.write(0x0224, 0xA9); // LDA #$00        Z=1
	bus.write(0x0225, 0x00); //     ^
	bus.write(0x0226, 0xD0); // BNE +2
	bus.write(0x0227, 0x02); //     ^
	bus.write(0x0228, 0xA2); // LDX #$DE
	bus.write(0x0229, 0xDE); //     ^
	bus.write(0x022A, 0x86); // STX $83
	bus.write(0x022B, 0x83); //     ^

	// === BVS: V=1 -> should be TAKEN. Expect mem[$84] == 0xAA ===
	bus.write(0x022C, 0xA2); // LDX #$AA
	bus.write(0x022D, 0xAA); //     ^
	bus.write(0x022E, 0x18); // CLC             force carry-in = 0 for a clean overflow
	bus.write(0x022F, 0xA9); // LDA #$7F
	bus.write(0x0230, 0x7F); //     ^
	bus.write(0x0231, 0x69); // ADC #$01        0x7F+1 -> signed overflow, V=1, N=1
	bus.write(0x0232, 0x01); //     ^
	bus.write(0x0233, 0x70); // BVS +2
	bus.write(0x0234, 0x02); //     ^
	bus.write(0x0235, 0xA2); // LDX #$DE
	bus.write(0x0236, 0xDE); //     ^
	bus.write(0x0237, 0x86); // STX $84
	bus.write(0x0238, 0x84); //     ^

	// === BVC: V=1 -> should be NOT TAKEN. Expect mem[$85] == 0xDE ===
	bus.write(0x0239, 0xA2); // LDX #$AA
	bus.write(0x023A, 0xAA); //     ^
	bus.write(0x023B, 0x18); // CLC
	bus.write(0x023C, 0xA9); // LDA #$7F
	bus.write(0x023D, 0x7F); //     ^
	bus.write(0x023E, 0x69); // ADC #$01        V=1
	bus.write(0x023F, 0x01); //     ^
	bus.write(0x0240, 0x50); // BVC +2
	bus.write(0x0241, 0x02); //     ^
	bus.write(0x0242, 0xA2); // LDX #$DE
	bus.write(0x0243, 0xDE); //     ^
	bus.write(0x0244, 0x86); // STX $85
	bus.write(0x0245, 0x85); //     ^

	// === BMI: N=1 -> should be TAKEN. Expect mem[$86] == 0xAA ===
	bus.write(0x0246, 0xA2); // LDX #$AA
	bus.write(0x0247, 0xAA); //     ^
	bus.write(0x0248, 0x18); // CLC
	bus.write(0x0249, 0xA9); // LDA #$7F
	bus.write(0x024A, 0x7F); //     ^
	bus.write(0x024B, 0x69); // ADC #$01        N=1
	bus.write(0x024C, 0x01); //     ^
	bus.write(0x024D, 0x30); // BMI +2
	bus.write(0x024E, 0x02); //     ^
	bus.write(0x024F, 0xA2); // LDX #$DE
	bus.write(0x0250, 0xDE); //     ^
	bus.write(0x0251, 0x86); // STX $86
	bus.write(0x0252, 0x86); //     ^

	// === BPL: N=1 -> should be NOT TAKEN. Expect mem[$87] == 0xDE ===
	bus.write(0x0253, 0xA2); // LDX #$AA
	bus.write(0x0254, 0xAA); //     ^
	bus.write(0x0255, 0x18); // CLC
	bus.write(0x0256, 0xA9); // LDA #$7F
	bus.write(0x0257, 0x7F); //     ^
	bus.write(0x0258, 0x69); // ADC #$01        N=1
	bus.write(0x0259, 0x01); //     ^
	bus.write(0x025A, 0x10); // BPL +2
	bus.write(0x025B, 0x02); //     ^
	bus.write(0x025C, 0xA2); // LDX #$DE
	bus.write(0x025D, 0xDE); //     ^
	bus.write(0x025E, 0x86); // STX $87
	bus.write(0x025F, 0x87); //     ^

	// === SBC: no-borrow case (C=1 going in). Expect mem[$90] == 0x40 ===
	bus.write(0x0260, 0xA9); // LDA #$FF
	bus.write(0x0261, 0xFF); //     ^
	bus.write(0x0262, 0x69); // ADC #$01        forces C=1
	bus.write(0x0263, 0x01); //     ^
	bus.write(0x0264, 0xA9); // LDA #$50        A=0x50
	bus.write(0x0265, 0x50); //     ^
	bus.write(0x0266, 0xE9); // SBC #$10        0x50-0x10-0(no borrow, C was 1) = 0x40, C=1
	bus.write(0x0267, 0x10); //     ^
	bus.write(0x0268, 0x85); // STA $90
	bus.write(0x0269, 0x90); //     ^

	// === SBC: borrow case (C=0 going in). Expect mem[$91] == 0xEF ===
	bus.write(0x026A, 0x18); // CLC             force C=0
	bus.write(0x026B, 0xA9); // LDA #$10        A=0x10
	bus.write(0x026C, 0x10); //     ^
	bus.write(0x026D, 0xE9); // SBC #$20        0x10-0x20-1(borrow) = 0xEF (-17), C=0, N=1
	bus.write(0x026E, 0x20); //     ^
	bus.write(0x026F, 0x85); // STA $91
	bus.write(0x0270, 0x91); //     ^

	// === AND. Expect mem[$92] == 0x3C ===
	bus.write(0x0271, 0xA9); // LDA #$FF
	bus.write(0x0272, 0xFF); //     ^
	bus.write(0x0273, 0x29); // AND #$3C        0xFF & 0x3C = 0x3C, Z=0, N=0
	bus.write(0x0274, 0x3C); //     ^
	bus.write(0x0275, 0x85); // STA $92
	bus.write(0x0276, 0x92); //     ^

	// === ASL, accumulator mode. Expect mem[$94] == 0x82 ===
	bus.write(0x0277, 0xA9); // LDA #$41
	bus.write(0x0278, 0x41); //     ^
	bus.write(0x0279, 0x0A); // ASL A           0x41<<1 = 0x82, C=0 (old bit7 was 0)
	bus.write(0x027A, 0x85); // STA $94
	bus.write(0x027B, 0x94); //     ^

	// === ASL, zero page, on pre-seeded 0x82. Expect mem[$95] == 0x04 ===
	bus.write(0x027C, 0x06); // ASL $95         0x82<<1 truncated to 0x04, C=1 (old bit7 was 1)
	bus.write(0x027D, 0x95); //     ^

	// === Clear-flag instructions ===
	bus.write(0x027E, 0x18); // CLC
	bus.write(0x027F, 0xA9); // LDA #$7F
	bus.write(0x0280, 0x7F); //     ^
	bus.write(0x0281, 0x69); // ADC #$01        sets V=1, N=1 fresh, for CLV to act on
	bus.write(0x0282, 0x01); //     ^
	bus.write(0x0283, 0xB8); // CLV             status V bit -> 0
	bus.write(0x0284, 0x58); // CLI             status I bit -> 0
	bus.write(0x0285, 0xD8); // CLD             status D bit -> 0

	bus.write(0x0286, 0x4C); // JMP $0286       infinite loop, marks clean end
	bus.write(0x0287, 0x86); //     ^ lo
	bus.write(0x0288, 0x02); //     ^ hi
	// - end -   hardcoded test program

	bus.cpu.reset();
}

void Emulator::clock() {
	bus.cpu.clock();
}
