#include "Inputs.hpp"
#include "Logging.hpp"

Inputs::Inputs(): _quit(false) {
	for (int i = 0; i < Inputs::nb_input; i++) {
		_key_status[i] = false;
	}
	setKeys();
}

Inputs::~Inputs() {}

bool				Inputs::getKey(InputType::Enum type) {
	return _key_status[static_cast<int>(type)];
}

void				Inputs::setKeys() {
	_input_key_map = {
		{ SDL_SCANCODE_W, InputType::Enum::UP },
		{ SDL_SCANCODE_S, InputType::Enum::DOWN },
		{ SDL_SCANCODE_A, InputType::Enum::LEFT },
		{ SDL_SCANCODE_D, InputType::Enum::RIGHT },
		{ SDL_SCANCODE_SPACE, InputType::Enum::ACTION },
		{ SDL_SCANCODE_RETURN, InputType::Enum::CONFIRM },
		{ SDL_SCANCODE_ESCAPE, InputType::Enum::CANCEL }
	};
	logDebug("set keys");
}

bool				Inputs::shouldQuit() {
	return (_quit);
}

const glm::ivec2	&Inputs::getMousePos() {
	return (_mouse_pos);
}

const glm::ivec2	&Inputs::getMouseRel() {
	return (_mouse_rel);
}

void				Inputs::update() {
	SDL_Event		event;
	SDL_Scancode	scan;

	_mouse_rel.x = 0;
	_mouse_rel.y = 0;
	while (SDL_PollEvent(&event)) {
		switch (event.type) {
		case SDL_KEYDOWN:
			scan = event.key.keysym.scancode;
			try {
				int index = static_cast<int>(_input_key_map.at(scan));
				_key_status[index] = true;
			}
			catch(std::out_of_range oor) {
				continue;
			}
			break;
		case SDL_KEYUP:
			scan = event.key.keysym.scancode;
			try {
				int index = static_cast<int>(_input_key_map.at(scan));
				_key_status[index] = false;
			}
			catch(std::out_of_range oor) {
				continue;
			}
			break;
		case SDL_MOUSEMOTION:
			_mouse_pos.x = event.motion.x;
			_mouse_pos.y = event.motion.y;
			_mouse_rel.x = event.motion.xrel;
			_mouse_rel.y = event.motion.yrel;
			break;
		case SDL_WINDOWEVENT:
			_quit = event.window.type == SDL_WINDOWEVENT_CLOSE;
			break;
		case SDL_QUIT:
			_quit = true;
			break;
		default:
			break;
		}
	}
}
