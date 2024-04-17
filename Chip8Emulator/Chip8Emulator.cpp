#include <iostream>
#include <string>
#include <SDL.h>

#include "SDLDisplay.h"
#include "Hardware.h"

#define INSTRUCTIONS_PER_SECOND 70

bool checkFilename(int argc, char* argv[]);

int main(int argc, char *argv[]) {
	if (!checkFilename(argc, argv)) return 1;

	bool isRunning = true;
	SDLDisplay::SDLDisplay* display = new SDLDisplay::SDLDisplay();
	Hardware::Hardware* chip8 = new Hardware::Hardware(display, false);

	SDL_Event ev;


	display->initDisplay();
	if (!chip8->loadProgram(argv[1])) return 1;

	int tickDelay = 1000 / INSTRUCTIONS_PER_SECOND;  //1000 ms in a sec, divide by number of instructions
	

	while (isRunning) {
		// Get initial tick
		Uint32 previousTick = SDL_GetTicks();

		// Poll events
		while (SDL_PollEvent(&ev) != 0) {
			if (ev.type == SDL_QUIT) isRunning = false;
		}
		// Process opcode
		chip8->processInstruction();

		// Update display
		display->updateWindow();

		// Delay if necessary
		Uint32 currentTick = SDL_GetTicks();
		if (currentTick - previousTick < tickDelay) {
			SDL_Delay(tickDelay - (currentTick - previousTick));
			
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