#include "SDLDisplay.h"
#include <iostream>

SDLDisplay::SDLDisplay::SDLDisplay(){
	window = nullptr;
	windowSurface = nullptr;
	drawingSurface = nullptr;
	blankSurface = nullptr;
	this->renderer = nullptr;

	displayMemory[WIDTH-1][HEIGHT-1] = {};
}

void SDLDisplay::SDLDisplay::initDisplay() {
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
		std::cout << "Video initialization error: " << SDL_GetError() << std::endl;
	else {
		this->window = SDL_CreateWindow("Chip8", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH*SCREEN_SCALE, HEIGHT*SCREEN_SCALE, SDL_WINDOW_SHOWN);
		if (this->window == NULL)
			std::cout << "Window creation error: " << SDL_GetError() << std::endl;
		else {
			
			this->renderer = SDL_CreateRenderer(this->window, -1, 0);

			SDL_UpdateWindowSurface(window);
			
			int test = 0;
		}
	}
}

void SDLDisplay::SDLDisplay::destroyDisplay() {
	SDL_DestroyWindow(window);
	SDL_Quit();
}

void SDLDisplay::SDLDisplay::updateWindow() {
	SDL_BlitSurface(this->drawingSurface, NULL, this->windowSurface, NULL);
	SDL_UpdateWindowSurface(window);
}

void SDLDisplay::SDLDisplay::clearScreen() {
	for (int x = 0; x < WIDTH; x++) {
		for (int y = 0; y < HEIGHT; y++) {
			displayMemory[x][y] = 0;
		}
	}

	SDL_SetRenderDrawColor(this->renderer, 0, 0, 0, 255);
	SDL_RenderClear(this->renderer);
}

bool SDLDisplay::SDLDisplay::drawPixel(short x, short y) {
	bool alreadyExists = false;

	if (this->displayMemory[x][y] == 1) {
		alreadyExists = true;
		this->displayMemory[x][y] == 0;
	}
	else {
		this->displayMemory[x][y] == 1;
	}

	SDL_Rect* pixel = new SDL_Rect;
	pixel->x = x*SCREEN_SCALE;
	pixel->y = y*SCREEN_SCALE;
	pixel->w = SCREEN_SCALE;
	pixel->h = SCREEN_SCALE;


	if (!alreadyExists) { SDL_SetRenderDrawColor(this->renderer, 255, 255, 255, 255); }
	else { SDL_SetRenderDrawColor(this->renderer, 0, 0, 0, 255); }
	SDL_RenderFillRect(this->renderer, pixel);
	//SDL_SetRenderDrawColor(this->renderer, 0, 0, 0, 255);
	SDL_RenderPresent(this->renderer);

	return alreadyExists;
}