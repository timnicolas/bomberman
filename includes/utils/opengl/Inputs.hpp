#ifndef INPUTS_HPP
# define INPUTS_HPP

#include <SDL2/SDL.h>
#include <map>
#include <array>
#include <unordered_set>
#include "useGlm.hpp"
#include "SettingsJson.hpp"

// equivalent of NULL for scancode
#define NO_SCANCODE SDL_SCANCODE_F24

/**
 * @brief this is the list of all user-defined inputs
 */
namespace InputType {
	enum Enum {
		NO_KEY = -1,
		UP = 0,
		DOWN,
		LEFT,
		RIGHT,
		ACTION,
		CONFIRM,
		CANCEL,
		GOTO_MENU,
		SHOW_HELP,
		NB_INPUTS  // need to be the last element
	};
}  // namespace InputType

/**
 * @brief static class to manage input
 *
 * With this class, you can:
 *  - set key used for all InputType
 *  - get the state of a key | the mouse
 */
class Inputs {
public:
	static const int						nb_input = InputType::NB_INPUTS;
	static const std::string				input_type_name[Inputs::nb_input];
	static const std::string				_conf_file;

	~Inputs();

	static Inputs							&get();
	static bool								getKey(InputType::Enum type);
	static bool								getKeyUp(InputType::Enum type);
	static bool								getKeyDown(InputType::Enum type);
	static std::string						getKeyName(InputType::Enum type);
	static void								configureKey(InputType::Enum type);
	static void								cancelConfiguration();
	static bool								getKeyByScancode(SDL_Scancode scancode);
	static bool								getKeyByScancodeUp(SDL_Scancode scancode);
	static bool								getKeyByScancodeDown(SDL_Scancode scancode);
	static std::string						getScancodeName(SDL_Scancode scancode);
	static bool								shouldQuit();
	static const glm::ivec2					&getMousePos();
	static const glm::ivec2					&getMouseRel();
	static bool								getRightClick();
	static bool								getLeftClick();
	static bool								getRightClickUp();
	static bool								getLeftClickUp();
	static bool								getRightClickDown();
	static bool								getLeftClickDown();
	static void								update();
	static std::string						getInputKeyName(InputType::Enum type);
	static bool								isConfiguring();

private:
	Inputs();
	Inputs(const Inputs &src);
	Inputs 									&operator=(const Inputs &src);

	bool									_getKey(InputType::Enum type) const;
	bool									_getKeyUp(InputType::Enum type) const;
	bool									_getKeyDown(InputType::Enum type) const;
	std::string								_getKeyName(InputType::Enum type) const;
	void									_configureKey(InputType::Enum type);
	void									_cancelConfiguration();
	bool									_getKeyByScancode(SDL_Scancode scancode) const;
	bool									_getKeyByScancodeUp(SDL_Scancode scancode) const;
	bool									_getKeyByScancodeDown(SDL_Scancode scancode) const;
	std::string								_getScancodeName(SDL_Scancode scancode) const;
	bool									_shouldQuit() const;
	const glm::ivec2						&_getMousePos() const;
	const glm::ivec2						&_getMouseRel() const;
	bool									_getRightClick() const;
	bool									_getLeftClick() const;
	bool									_getRightClickUp() const;
	bool									_getLeftClickUp() const;
	bool									_getRightClickDown() const;
	bool									_getLeftClickDown() const;
	void									_update();
	std::string								_getInputKeyName(InputType::Enum type);
	bool									_isConfiguring();

	bool									_configuring;
	InputType::Enum							_next_action_type;
	bool									_quit;
	glm::ivec2								_mouse_pos;
	glm::ivec2								_mouse_rel;
	bool									_left_click;
	bool									_right_click;
	bool									_left_click_previous;
	bool									_right_click_previous;
	std::map<SDL_Scancode, InputType::Enum>	_input_key_map;
	std::unordered_set<int64_t>				_used_scan;
	bool									_key_status[Inputs::nb_input];
	bool									_key_previous_status[Inputs::nb_input];
	SettingsJson							_controls;
	std::vector<SDL_Scancode>				_scancodes_previous;
	std::vector<SDL_Scancode>				_scancodes_pressed;
};

#endif
