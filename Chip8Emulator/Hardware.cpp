#include "Hardware.h"

Hardware::Hardware::Hardware(SDLDisplay::SDLDisplay* display, Keyboard::Keyboard* keyboard, bool isOldShift) {
	this->display = display;
	this->indexPointer = 0;
	this->opcode = 0;
	this->programCounter = 0;
	this->stack = new std::stack<short>();
	this->oldStyleShift = isOldShift;
	this->keyboard = keyboard;

	this->delayTimer = 0;
	this->soundTimer = 0;

	// Change this to configurable if there are compatibility issues later
	this->oldStyleJump = true;

	initializeFont();

	srand(time(NULL));
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
	unsigned short nnn, tempValue;  //Temp value is used for one offs - quick storage of computed index and random number

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
		case 0x00ee:
			this->programCounter = this->stack->top();
			this->stack->pop();
			break;
		}
		break;
	case 1:
		this->programCounter = nnn;
		break;
	case 10:
		this->indexPointer = nnn;
		break;
	case 11:
		if (oldStyleJump) { programCounter = vRegister[0] + nnn; }
		else { programCounter = vRegister[xNibble] + nnn; }
		break;
	case 12:
		tempValue = std::rand() % nnByte;
		vRegister[xNibble] = tempValue & nnByte;
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
	case 14:
		switch (nnByte) {
		case 0x009e:
			if (keyboard->getKeyDownStatus(vRegister[xNibble])) { programCounter += 2; }
			break;
		case 0x00a1:
			if (!keyboard->getKeyDownStatus(vRegister[xNibble])) { programCounter += 2; }
			break;
		}
		break;
	case 15:
		switch (nnByte) {
		case 0x0007:
			vRegister[xNibble] = this->delayTimer;
			break;
		case 0x0015:
			this->delayTimer = vRegister[xNibble];
			break;
		case 0x0018:
			this->soundTimer = vRegister[xNibble];
			break;
		case 0x001e:
			tempValue = 0;
			indexPointer += vRegister[xNibble];
			if (indexPointer >= 4096) {
				//Simulating overflow
				indexPointer -= 4096;
				tempValue = 1;
			}
			vRegister[15] = tempValue;
			break;
		case 0x000a:
			tempValue = keyboard->getFirstKey();
			if (tempValue == -1) { programCounter -= 2; } //This simulates a blocking instruction, wait for keypress
			else { vRegister[xNibble] = tempValue; }
			break;
		case 0x0029:
			indexPointer = vRegister[xNibble] * 5;  // pointing to character in memory.  memory[0] is the first font, each fontCharacter is 5 bytes long
			break;
		case 0x0033:
			tempValue = vRegister[xNibble];
			memory[indexPointer] = tempValue / 100;
			memory[indexPointer + 1] = (tempValue / 10) % 10;
			memory[indexPointer + 2] = tempValue % 10;
			break;
		case 0x0055:
			for (int i = 0; i <= xNibble; i++) {
				memory[indexPointer + i] = vRegister[i];
			}
			break;
		case 0x0065:
			for (int i = 0; i <= xNibble; i++) {
				vRegister[i] = memory[indexPointer + i];
			}
			break;
		}
		break;
	case 2:
		this->stack->push(this->programCounter);
		this->programCounter = nnn;
		break;
	case 3:
		if (vRegister[xNibble] == nnByte) programCounter += 2;
		break;
	case 4:
		if (!vRegister[xNibble] == nnByte) programCounter += 2;
		break;
	case 5:
		if (vRegister[xNibble] == vRegister[yNibble]) programCounter += 2;
		break;
	case 6:
		vRegister[xNibble] = nnByte;
		break;
	case 7:
		vRegister[xNibble] = (vRegister[xNibble] + nnByte) & 0x00FF;
		break;
	case 8:
		switch (nNibble) {
		case 0:
			vRegister[xNibble] = vRegister[yNibble];
			break;
		case 1:
			vRegister[xNibble] |= vRegister[yNibble];
			break;
		case 2:
			vRegister[xNibble] &= vRegister[yNibble];
			break;
		case 3:
			vRegister[xNibble] ^= vRegister[yNibble];
			break;
		case 4:
			tempValue = 0;
			if (vRegister[xNibble] + vRegister[yNibble] > 255) tempValue = 1;
			vRegister[xNibble] = (vRegister[xNibble] + vRegister[yNibble]) & 0x00FF;
			vRegister[15] = tempValue;
			break;
		case 5:
			tempValue = 1;
			if (vRegister[xNibble] < vRegister[yNibble]) tempValue = 0;
			vRegister[xNibble] = (vRegister[xNibble] - vRegister[yNibble]) & 0x00FF;
			vRegister[15] = tempValue;
			break;
		case 6:
			tempValue = 0;
			if (this->oldStyleShift == true) vRegister[xNibble] = vRegister[yNibble];
			if ((vRegister[xNibble] & 0x1) == 1) tempValue = 1;
			vRegister[xNibble] >>= 1;
			vRegister[15] = tempValue;
			break;
		case 7:
			tempValue = 1;
			if (vRegister[yNibble] < vRegister[xNibble]) tempValue = 0;
			vRegister[xNibble] = (vRegister[yNibble] - vRegister[xNibble]) & 0x00FF;
			vRegister[15] = tempValue;
			break;
		case 14:
			tempValue = 0;
			if (this->oldStyleShift == true) vRegister[xNibble] = vRegister[yNibble];
			tempValue = (vRegister[xNibble] >> 7) & 0x1;
			vRegister[xNibble] <<= 1;
			vRegister[15] = tempValue;
			break;
		}
		break;
	case 9:
		if (vRegister[xNibble] != vRegister[yNibble]) programCounter += 2;
		break;
	}

	return;
}

void Hardware::Hardware::decrementTimers() {
	if (this->delayTimer > 0) { this->delayTimer -= 1; }
	if (this->soundTimer > 0) { this->soundTimer -= 1; }
}

void Hardware::Hardware::initializeFont() {
	unsigned char fontCharacters[80] = {
		0xF0, 0x90, 0x90, 0x90, 0xF0,  // 0
		0x20, 0x60, 0x20, 0x20, 0x70,  // 1
		0xF0, 0x10, 0xF0, 0x80, 0xF0,  // 2
		0xF0, 0x10, 0xF0, 0x10, 0xF0,  // 3
		0x90, 0x90, 0xF0, 0x10, 0x10,  // 4
		0xF0, 0x80, 0xF0, 0x10, 0xF0,  // 5
		0xF0, 0x80, 0xF0, 0x90, 0xF0,  // 6
		0xF0, 0x10, 0x20, 0x40, 0x40,  // 7
		0xF0, 0x90, 0xF0, 0x90, 0xF0,  // 8
		0xF0, 0x90, 0xF0, 0x10, 0xF0,  // 9
		0xF0, 0x90, 0xF0, 0x90, 0x90,  // A
		0xE0, 0x90, 0xE0, 0x90, 0xE0,  // B
		0xF0, 0x80, 0x80, 0x80, 0xF0,  // C
		0xE0, 0x90, 0x90, 0x90, 0xE0,  // D
		0xF0, 0x80, 0xF0, 0x80, 0xF0,  // E
		0xF0, 0x80, 0xF0, 0x80, 0x80   // F
	};
		// place the fonts into memory
	memcpy(memory, fontCharacters, sizeof(fontCharacters));
	
}