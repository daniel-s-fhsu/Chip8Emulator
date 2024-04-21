#pragma once
#include "SDLDisplay.h"
#include "Keyboard.h"
#include <fstream>
#include <iostream>
#include <bitset>
#include <stack>
#include <cstdlib>

namespace Hardware {
	class Hardware {
	public:
		Hardware(SDLDisplay::SDLDisplay* display, Keyboard::Keyboard* keyboard, bool isOldShift);
		bool loadProgram(char* filename);
		bool processInstruction();
		void decrementTimers();
	private:
		unsigned short opcode;
		unsigned char memory[4096]{};
		unsigned char vRegister[16]{};
		unsigned short indexPointer;
		unsigned short programCounter;

		unsigned short delayTimer;
		unsigned short soundTimer;

		bool oldStyleShift;
		bool oldStyleJump;

		std::stack<short>* stack;
		SDLDisplay::SDLDisplay* display;
		Keyboard::Keyboard* keyboard;

		void fetchOpcode();
		void decodeOpcode();

		void initializeFont();
	};
}