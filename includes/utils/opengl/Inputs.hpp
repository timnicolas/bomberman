#ifndef INPUTS_HPP
#define INPUTS_HPP

#include <SDL2/SDL.h>
#include <map>
#include <array>
#include <unordered_set>
#include "useGlm.hpp"
#include "SettingsJson.hpp"

// equivalent of NULL for scancode & keycode
#define NO_SCANCODE	SDL_SCANCODE_F24
#define NO_KEYCODE	SDLK_F24

// default input value
#define DEFAULT_UP			SDL_SCANCODE_UP
#define DEFAULT_DOWN		SDL_SCANCODE_DOWN
#define DEFAULT_LEFT		SDL_SCANCODE_LEFT
#define DEFAULT_RIGHT		SDL_SCANCODE_RIGHT
#define DEFAULT_ACTION		SDL_SCANCODE_SPACE
#define DEFAULT_ACTION_2	SDL_SCANCODE_B
#define DEFAULT_CONFIRM		SDL_SCANCODE_RETURN
#define DEFAULT_CANCEL		SDL_SCANCODE_ESCAPE
#define DEFAULT_MENU		SDL_SCANCODE_TAB
#define DEFAULT_HELP		SDL_SCANCODE_F1
#define DEFAULT_CHEATCODE	SDL_SCANCODE_SLASH

namespace InputType {
	/**
	 * @brief this is the list of all user-defined inputs
	 */
	enum Enum {
		NO_KEY = -1,
		UP = 0,
		DOWN,
		LEFT,
		RIGHT,
		ACTION,
		ACTION_2,
		CONFIRM,
		CANCEL,
		GOTO_MENU,
		SHOW_HELP,
		CHEAT_CODE,
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
	static const int						nb_input = InputType::NB_INPUTS;  /**< Number of inputs */
	static const std::string				input_type_name[Inputs::nb_input];  /**< Inputs names */
	static const std::string				configFile;  /**< File with inputs keys */
	static const SDL_Scancode				default_keys[InputType::NB_INPUTS];  /**< Inputs default keys */

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
	static const glm::ivec2					&getMouseScroll();
	static bool								getRightClick();
	static bool								getLeftClick();
	static bool								getRightClickUp();
	static bool								getLeftClickUp();
	static bool								getRightClickDown();
	static bool								getLeftClickDown();
	static void								update();
	static void								resetKeys();
	static bool								isConfiguring();
	static void								setTextInputMode(bool enable);
	static SDL_Keycode						getTextInputKeycode();
	static std::string						getTextInputString();

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
	const glm::ivec2						&_getMouseScroll() const;
	bool									_getRightClick() const;
	bool									_getLeftClick() const;
	bool									_getRightClickUp() const;
	bool									_getLeftClickUp() const;
	bool									_getRightClickDown() const;
	bool									_getLeftClickDown() const;
	void									_update();
	void									_resetKeys();
	bool									_isConfiguring();
	void									_setTextInputMode(bool enable);
	SDL_Keycode								_getTextInputKeycode() const;
	std::string								_getTextInputString() const;
	bool									_ingoreScancode(SDL_Scancode scan) const;
	bool									_ignoreInputType(InputType::Enum type) const;

	bool									_configuring;  /**< If an inputs is configuring */
	InputType::Enum							_next_action_type;  /**< Used in key configuration */
	bool									_quit;  /**< Quit game */
	glm::ivec2								_mouse_pos;  /**< Mouse position */
	glm::ivec2								_mouse_rel;  /**< Mouse movement (relative to last position) */
	glm::ivec2								_scroll_rel;  /**< Mouse scroll (relative to last scroll position) */
	bool									_left_click;  /**< If left click pressed */
	bool									_right_click;  /**< If right click released */
	bool									_left_click_previous;  /**< Used to know if we are in the first click frame */
	bool									_right_click_previous;  /**< Used to know if we are in the first click frame */
	std::map<SDL_Scancode, InputType::Enum>	_input_key_map;  /**< scancode associated to inputs */
	std::unordered_set<int64_t>				_used_scan;  /**< All scancodes used for inputs */
	bool									_key_status[Inputs::nb_input];  /**< All inputs status (pressed or released) */
	bool									_key_previous_status[Inputs::nb_input];  /**< To know if it's the press frame */
	SettingsJson							_controls;  /**< All controls (to save in file) */
	std::vector<SDL_Scancode>				_scancodes_pressed;  /**< All scancodes pressed */
	std::vector<SDL_Scancode>				_scancodes_previous;  /**< To know if it's the press frame */

	bool									_isTextInputMode;  /**< True if we are in "textInput" mode */
	SDL_Keycode								_lastKeycode;  /**< The last keycode pressed */
	std::string								_currentText;  /**< The current text ("tab", "e", " ", ...) */
	std::vector<SDL_Scancode>				_textInputIgnore;  /**< Scancodes ignored by textInput */
};

#endif
