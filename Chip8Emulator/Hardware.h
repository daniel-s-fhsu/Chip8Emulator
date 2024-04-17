#pragma once
#include "SDLDisplay.h"
#include <fstream>
#include <iostream>
#include <bitset>
#include <stack>

namespace Hardware {
	class Hardware {
	public:
		Hardware(SDLDisplay::SDLDisplay* display, bool isOldShift);
		bool loadProgram(char* filename);
		bool processInstruction();
	private:
		unsigned short opcode;
		unsigned char memory[4096]{};
		unsigned char vRegister[16]{};
		unsigned short indexPointer;
		unsigned short programCounter;

		bool oldStyleShift;

		std::stack<short>* stack;
		SDLDisplay::SDLDisplay* display;

		void fetchOpcode();
		void decodeOpcode();
	};
}