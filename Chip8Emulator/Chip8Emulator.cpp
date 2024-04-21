#include <iostream>
#include <string>
#include <SDL.h>

#include "SDLDisplay.h"
#include "Hardware.h"
#include "Keyboard.h"

#define INSTRUCTIONS_PER_SECOND 800  // This value can be modified to change speed of emulation
#define TIMER_SPEED 60  // Chip 8 runs timers at 60 hz, this value should not be changed

bool checkFilename(int argc, char* argv[]);

int main(int argc, char *argv[]) {
	if (!checkFilename(argc, argv)) return 1;

	bool isRunning = true;
	SDLDisplay::SDLDisplay* display = new SDLDisplay::SDLDisplay();
	Keyboard::Keyboard* keyboard = new Keyboard::Keyboard();
	Hardware::Hardware* chip8 = new Hardware::Hardware(display, keyboard, false);

	SDL_Event ev;


	display->initDisplay();
	if (!chip8->loadProgram(argv[1])) return 1;

	int tickDelay = 1000 / INSTRUCTIONS_PER_SECOND;  //1000 ms in a sec, divide by number of instructions
	int timerDelay = 1000 / TIMER_SPEED;
	

	while (isRunning) {
		// Get initial tick
		Uint32 previousTick = SDL_GetTicks();

		// Poll events
		while (SDL_PollEvent(&ev) != 0) {
			if (ev.type == SDL_QUIT) isRunning = false;
			if (ev.type == SDL_KEYDOWN || ev.type == SDL_KEYUP) keyboard->handleKeyboard(&ev);
		}
		// Process opcode
		chip8->processInstruction();

		// Update display
		//display->updateWindow();

		// Delay if necessary
		Uint32 currentTick = SDL_GetTicks();
		if (currentTick - previousTick < tickDelay) {
			SDL_Delay(tickDelay - (currentTick - previousTick));	
		}

		// Update hardware timers
		if (currentTick - previousTick < timerDelay) {
			chip8->decrementTimers();
		}

	}

	display->destroyDisplay();
	std::cout << "Destroyed display safely\n";
	return 0;
}

bool checkFilename(int argc, char* argv[]) {
	if (!(argc > 1)) {
		std::cout << "Usage: chip8.exe \"filename.ch8\"" << std::endl;
		return false;
	}
	std::string filename = argv[1];

	if (filename.substr(filename.find_last_of(".") + 1) == "ch8") {
		return true;
	} else {
		std::cout << "Requires a .ch8 file" << std::endl;
	}
	return false;
}