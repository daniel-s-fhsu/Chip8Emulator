#pragma once
#include <SDL.h>
#include <vector>
#include <algorithm>

namespace Keyboard {
	class Keyboard {
	public:
		Keyboard();
		void handleKeyboard(SDL_Event* event);
		bool getKeyDownStatus(short keyname);
		short getFirstKey();
	private:
		std::vector<short>* pressedKeys;
	};
}