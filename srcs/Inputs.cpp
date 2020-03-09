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

Inputs::Inputs(): _configuring(false), _quit(false), _left_click(false), _right_click(false) {
	for (int i = 0; i < Inputs::nb_input; i++) {
		_key_status[i] = false;
		_key_previous_status[i] = false;
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

/**
	Return a reference to singleton Inputs.

	@return the reference to the singleton.
*/
Inputs				&Inputs::get() {
	static Inputs		instance;
	return (instance);
}

/**
	Return the state of the key corresponding to the action passed.

	@param type The type defining the action to verify.
	@return The state of the key (true == pressed).
*/
bool				Inputs::getKey(InputType::Enum type) {
	return (Inputs::get()._getKey(type));
}

bool				Inputs::_getKey(InputType::Enum type) const {
	return _key_status[static_cast<int>(type)];
}

/**
	Indicate if the key corresponding to the action passed has just been released.

	@param type The type defining the action to verify.
	@return true if the key has been released.
*/
bool				Inputs::getKeyUp(InputType::Enum type) {
	return (Inputs::get()._getKeyUp(type));
}

bool				Inputs::_getKeyUp(InputType::Enum type) const {
	int		index = static_cast<int>(type);
	return (!_key_status[index] && _key_previous_status[index]);
}

/**
	Indicate if the key corresponding to the action passed has just been pressed.

	@param type The type defining the action to verify.
	@return true if the key has been pressed.
*/
bool				Inputs::getKeyDown(InputType::Enum type) {
	return (Inputs::get()._getKeyDown(type));
}

bool				Inputs::_getKeyDown(InputType::Enum type) const {
	int		index = static_cast<int>(type);
	return (_key_status[index] && !_key_previous_status[index]);
}

/**
	Configure the action passed to be associated to the next key pressed.
	This function will change the execution of the update function.

	@param type The type defining the action to configure.
*/
void				Inputs::configureKey(InputType::Enum type) {
	Inputs::get()._configureKey(type);
}

void				Inputs::_configureKey(InputType::Enum type) {
	_configuring = true;
	_next_action_type = type;
	_used_scan.erase(_controls.j("keys").i(input_type_name[_next_action_type]));
}

/**
	Cancel the wait for a key to configure an action.
	This function will reset the execution of the update function to its normal state.
*/
void				Inputs::cancelConfiguration() {
	Inputs::get()._cancelConfiguration();
}

void				Inputs::_cancelConfiguration() {
	if (_configuring) {
		_configuring = false;
		_used_scan.insert(_controls.j("keys").i(input_type_name[_next_action_type]));
	}
}

/**
	Indicate if the window should be closed or not.

	@return true if the user tried to close the window, false otherwise.
*/
bool				Inputs::shouldQuit() {
	return (Inputs::get()._shouldQuit());
}

bool				Inputs::_shouldQuit() const {
	return (_quit);
}

/**
	Gives the current coordinates of the mouse.

	@return A reference to a constant vector containing the mouse position.
*/
const glm::ivec2	&Inputs::getMousePos() {
	return (Inputs::get()._getMousePos());
}

const glm::ivec2	&Inputs::_getMousePos() const {
	return (_mouse_pos);
}

/**
	Gives the offset of the mouse from the last poll of event.

	@return A reference to a constant vector containing the mouse offset.
*/
const glm::ivec2	&Inputs::getMouseRel() {
	return (Inputs::get()._getMouseRel());
}

const glm::ivec2	&Inputs::_getMouseRel() const {
	return (_mouse_rel);
}

/**
	Gives the right click (true if clicked, else false)

	@return The mouse state
*/
bool				Inputs::getRightClick() {
	return (Inputs::get()._getRightClick());
}

bool				Inputs::_getRightClick() const {
	return (_right_click);
}

/**
	Gives the left click (true if clicked, else false)

	@return The mouse state
*/
bool				Inputs::getLeftClick() {
	return (Inputs::get()._getLeftClick());
}

bool				Inputs::_getLeftClick() const {
	return (_left_click);
}

/**
	Indicate if the right click has just been released.

	@return true if the right click has just been released.
*/
bool				Inputs::getRightClickUp() {
	return (Inputs::get()._getRightClickUp());
}

bool				Inputs::_getRightClickUp() const {
	return (!_right_click && _right_click_previous);
}

/**
	Indicate if the left click has just been released.

	@return true if the left click has just been released.
*/
bool				Inputs::getLeftClickUp() {
	return (Inputs::get()._getLeftClickUp());
}

bool				Inputs::_getLeftClickUp() const {
	return (!_left_click && _left_click_previous);
}

/**
	Indicate if the right click has just been pressed.

	@return true if the right click has just been pressed.
*/
bool				Inputs::getRightClickDown() {
	return (Inputs::get()._getRightClickDown());
}

bool				Inputs::_getRightClickDown() const {
	return (_right_click && !_right_click_previous);
}

/**
	Indicate if the left click has just been pressed.

	@return true if the left click has just been pressed.
*/
bool				Inputs::getLeftClickDown() {
	return (Inputs::get()._getLeftClickDown());
}

bool				Inputs::_getLeftClickDown() const {
	return (_left_click && !_left_click_previous);
}

/**
	Poll all the SDL events from the last update call.
	This function should be called at the start of each frame in order for this class to have valid values.
*/
void				Inputs::update() {
	Inputs::get()._update();
}

void				Inputs::_update() {
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
					_key_previous_status[index] = _key_status[index];
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
				_key_previous_status[index] = _key_status[index];
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
		 case SDL_MOUSEBUTTONDOWN:
                switch (event.button.button) {
                    case SDL_BUTTON_LEFT:
						_left_click = true;
                        break;
                    case SDL_BUTTON_RIGHT:
						_right_click = true;
                        break;
                    default:
                        break;
                }
                break;
		 case SDL_MOUSEBUTTONUP:
                switch (event.button.button) {
                    case SDL_BUTTON_LEFT:
						_left_click = false;
                        break;
                    case SDL_BUTTON_RIGHT:
						_right_click = false;
                        break;
                    default:
                        break;
                }
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
