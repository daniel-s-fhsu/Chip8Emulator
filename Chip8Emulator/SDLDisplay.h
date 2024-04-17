#pragma once
#include <SDL.h>

#define WIDTH 64
#define HEIGHT 32
#define SCREEN_SCALE 20

namespace SDLDisplay {

	class SDLDisplay {
	public:
		SDLDisplay();
		void initDisplay();
		void destroyDisplay();
		void updateWindow();
		bool drawPixel(short x, short y);

		void clearScreen();
	private:
		SDL_Window* window;
		SDL_Renderer* renderer;
		SDL_Surface* windowSurface;
		SDL_Surface* drawingSurface;
		SDL_Surface* blankSurface;
		int displayMemory[WIDTH][HEIGHT];
	};
}