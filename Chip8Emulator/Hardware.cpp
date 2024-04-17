#include "Hardware.h"

Hardware::Hardware::Hardware(SDLDisplay::SDLDisplay* display) {
	this->display = display;
	this->indexPointer = 0;
	this->opcode = 0;
	this->programCounter = 0;
	this->stack = new std::stack<short>();
}

bool Hardware::Hardware::loadProgram(char* filePath) {
	std::ifstream inputFile{ filePath, std::ios::binary };

	if (!inputFile) {
		std::cout << "Failed to open the file: " << filePath << std::endl;
		return false;
	}

	// start loading program at 512 (0X0200).  Everything before that is interpreter space
	char byteLoader;
	int memoryIndex{ 512 };

	while (inputFile.get(byteLoader)) {
		memory[memoryIndex] = byteLoader;
		memoryIndex++;
	}

	// set the program counter to the start of program memory
	this->programCounter = 512;

	return true;
}

bool Hardware::Hardware::processInstruction() {
	// Fetch
	this->fetchOpcode();

	// Decode & execute
	this->decodeOpcode();


	return true;
}

void Hardware::Hardware::fetchOpcode() {
	this->opcode = this->memory[this->programCounter];
	this->opcode <<= 8;
	this->opcode |= this->memory[this->programCounter + 1];
	this->programCounter = programCounter + 2;
}

void Hardware::Hardware::decodeOpcode() {
	unsigned char xNibble, yNibble, nNibble, nnByte, instructionIndex;
	unsigned short nnn;

	xNibble = (this->opcode & 0x0F00) >> 8;
	yNibble = (this->opcode & 0x00F0) >> 4;
	nNibble = (this->opcode & 0x000F);
	nnByte = (this->opcode & 0x00FF);
	nnn = (this->opcode & 0x0FFF);
	instructionIndex = (this->opcode & 0xF000) >> 12;


	int x{};
	int y{};
	bool isCollision = false;


	switch (instructionIndex) {
	case 0:
		switch (nnn) {
		case 0x00e0:
			this->display->clearScreen();
			break;
		}
		break;
	case 1:
		this->programCounter = nnn;
		break;
	case 10:
		this->indexPointer = nnn;
		break;
	case 13:
		isCollision = false;
		vRegister[15] = 0;

		y = vRegister[yNibble] % 32;

		for (int i = 0; i < nNibble; i++) {
			unsigned char spriteData = this->memory[indexPointer + i];
			x = vRegister[xNibble] % 64;
			for (int j = 0; j < 8; j++) {
				if (x >= 64) break;
				short currentBit = (spriteData >> 7-j) & 0x01;  //reading the bits in reverse order - drawing sprite from left to right
				if (currentBit == 1) {
					if (!this->display->drawPixel(x, y)) isCollision = true;  //drawPixel returns true if there was a collision
				}
				x++;
			}
			y++;
			if (y >= 32) break;
		}

		if (isCollision) vRegister[15] = 1;
		break;
	case 6:
		this->vRegister[xNibble] = nnByte;
		break;
	case 7:
		this->vRegister[xNibble] += nnByte;
		break;
	}

	return;
}