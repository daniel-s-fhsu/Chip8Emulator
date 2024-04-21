#include "Keyboard.h"

Keyboard::Keyboard::Keyboard() {
	this->pressedKeys = new std::vector<short>();
}

void Keyboard::Keyboard::handleKeyboard(SDL_Event* event) {
	short keyCharacter;
	switch (event->key.keysym.scancode) {
	case 30: // '1'
		keyCharacter = 1;
		break;
	case 31:
		keyCharacter = 2;
		break;
	case 32:
		keyCharacter = 3;
		break;
	case 33:
		keyCharacter = 12;
		break;
	case 20:  // 'Q'
		keyCharacter = 4;
		break;
	case 26: // 'W'
		keyCharacter = 5;
		break;
	case 8: //'E'
		keyCharacter = 6;
		break;
	case 21: // 'R'
		keyCharacter = 13;
		break;
	case 4: // 'A'
		keyCharacter = 7;
		break;
	case 22: // 'S'
		keyCharacter = 8;
		break;
	case 7: // 'D'
		keyCharacter = 9;
		break;
	case 9: // 'F'
		keyCharacter = 14;
		break;
	case 29: // 'Z'
		keyCharacter = 10;
		break;
	case 27: // 'X'
		keyCharacter = 0;
		break;
	case 6: // 'C'
		keyCharacter = 11;
		break;
	case 25: // 'V'
		keyCharacter = 15;
		break;

	}

	if (event->type == SDL_KEYDOWN) {
		if (std::find(pressedKeys->begin(), pressedKeys->end(), keyCharacter) == pressedKeys->end()) {
			//Not found, so insert
			pressedKeys->push_back(keyCharacter);
		}
	}
	else if (event->type == SDL_KEYUP) {
		if (std::find(pressedKeys->begin(), pressedKeys->end(), keyCharacter) != pressedKeys->end()) {
			//found, so remove
			pressedKeys->erase(std::find(pressedKeys->begin(), pressedKeys->end(), keyCharacter));
		}
	}

}

bool Keyboard::Keyboard::getKeyDownStatus(short key) {
	// Returns true if key is in the pressedKeys vector
	if (std::find(pressedKeys->begin(), pressedKeys->end(), key) != pressedKeys->end()) {
		return true;
	}
	return false;
}

short Keyboard::Keyboard::getFirstKey() {
	//Returns the first key from pressedKeys, -1 if vector is empty
	if (pressedKeys->empty()) return -1;
	return pressedKeys->at(0);
}