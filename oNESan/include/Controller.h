#pragma once
#include <cstdint>

class Controller
{
public:
	Controller() {};

	enum Button {
		A      = (1 << 0),
		B      = (1 << 1),
		Select = (1 << 2),
		Start  = (1 << 3),
		Up     = (1 << 4),
		Down   = (1 << 5),
		Left   = (1 << 6),
		Right  = (1 << 7)
	};

	uint8_t input = 0x00;
	uint8_t shiftReg = input;

	uint8_t read() {
		if (latch)
			shiftReg = input;

		// rightmost bit
		uint8_t bit = (shiftReg & 0x01);
		shiftReg >>= 1;
		return bit;
	}

	void pressButton(Button button) {
		input |= button;
	}
	void releaseButton(Button button) {
		input &= ~button;
	}

	void setLatch(bool v) { 
		latch = v; 
		if (latch)
			shiftReg = input;
	}
private:
	bool latch = false;
};

