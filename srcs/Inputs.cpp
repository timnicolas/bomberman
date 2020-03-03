#include "Inputs.hpp"
#include "Logging.hpp"

const std::string	Inputs::input_type_name[] = {
	"up",
	"down",
	"left",
	"right",
	"action",
	"confirm",
	"cancel",
};
const std::string	Inputs::_conf_file = "configs/controls.json";

Inputs::Inputs(): _configuring(false), _quit(false) {
	for (int i = 0; i < Inputs::nb_input; i++) {
		_key_status[i] = false;
	}
	_controls.name("controls").description("controls settings");
	_controls.add<SettingsJson>("keys");
	_controls.j("keys").add<int64_t>("up", SDL_SCANCODE_W) \
		.setMin(4).setMax(286).setDescription("move up.");
	_controls.j("keys").add<int64_t>("down", SDL_SCANCODE_S) \
		.setMin(4).setMax(286).setDescription("move down.");
	_controls.j("keys").add<int64_t>("left", SDL_SCANCODE_A) \
		.setMin(4).setMax(286).setDescription("move left.");
	_controls.j("keys").add<int64_t>("right", SDL_SCANCODE_D) \
		.setMin(4).setMax(286).setDescription("move right.");
	_controls.j("keys").add<int64_t>("action", SDL_SCANCODE_SPACE) \
		.setMin(4).setMax(286).setDescription("action command.");
	_controls.j("keys").add<int64_t>("confirm", SDL_SCANCODE_RETURN) \
		.setMin(4).setMax(286).setDescription("confirm choice.");
	_controls.j("keys").add<int64_t>("cancel", SDL_SCANCODE_ESCAPE) \
		.setMin(4).setMax(286).setDescription("cancel choice.");
	try {
		if (!_controls.loadFile(Inputs::_conf_file)) {
			logWarn("Invalid value in " << Inputs::_conf_file << ".");
		}
	}
	catch(SettingsJson::SettingsException const & e) {
		logDebug("the file " << Inputs::_conf_file << " doesn't exist for now.");
	}
	_input_key_map = {
		{ static_cast<SDL_Scancode>(_controls.j("keys").i("up")), InputType::Enum::UP },
		{ static_cast<SDL_Scancode>(_controls.j("keys").i("down")), InputType::Enum::DOWN },
		{ static_cast<SDL_Scancode>(_controls.j("keys").i("left")), InputType::Enum::LEFT },
		{ static_cast<SDL_Scancode>(_controls.j("keys").i("right")), InputType::Enum::RIGHT },
		{ static_cast<SDL_Scancode>(_controls.j("keys").i("action")), InputType::Enum::ACTION },
		{ static_cast<SDL_Scancode>(_controls.j("keys").i("confirm")), InputType::Enum::CONFIRM },
		{ static_cast<SDL_Scancode>(_controls.j("keys").i("cancel")), InputType::Enum::CANCEL }
	};
	_used_scan = {
		_controls.j("keys").i("up"),
		_controls.j("keys").i("down"),
		_controls.j("keys").i("left"),
		_controls.j("keys").i("right"),
		_controls.j("keys").i("action"),
		_controls.j("keys").i("confirm"),
		_controls.j("keys").i("cancel"),
	};
	_controls.saveToFile(Inputs::_conf_file);
}

Inputs::~Inputs() {}

bool				Inputs::getKey(InputType::Enum type) {
	return _key_status[static_cast<int>(type)];
}

void				Inputs::setNextKey(InputType::Enum type) {
	_configuring = true;
	_next_action_type = type;
	_used_scan.erase(_controls.j("keys").i(input_type_name[_next_action_type]));
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
			if (!_configuring) {
				try {
					int index = static_cast<int>(_input_key_map.at(scan));
					_key_status[index] = true;
				}
				catch(std::out_of_range oor) {
					// unused key
					continue;
				}
			}
			else {
				if (_used_scan.find(scan) == _used_scan.end()) {
					_input_key_map.erase(static_cast<SDL_Scancode>(_controls.j("keys").i(input_type_name[_next_action_type])));
					_controls.j("keys").i(input_type_name[_next_action_type]) = scan;
					_used_scan.insert(scan);
					_input_key_map[scan] = _next_action_type;
					_controls.saveToFile(Inputs::_conf_file);
					_configuring = false;
					_key_status[static_cast<int>(_next_action_type)] = true;
					logInfo("Input '" << input_type_name[_next_action_type] << "' set.")
				}
				else {
					logWarn("This key is already used.");
				}
			}
			break;
		case SDL_KEYUP:
			scan = event.key.keysym.scancode;
			try {
				int index = static_cast<int>(_input_key_map.at(scan));
				_key_status[index] = false;
			}
			catch(std::out_of_range oor) {
				// unused key
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
