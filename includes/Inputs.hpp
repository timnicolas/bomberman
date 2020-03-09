#ifndef INPUTS_HPP
# define INPUTS_HPP

#include <SDL2/SDL.h>
#include <map>
#include <array>
#include <unordered_set>
#include "useGlm.hpp"
#include "SettingsJson.hpp"

namespace InputType {
	enum Enum {
		UP = 0,
		DOWN = 1,
		LEFT = 2,
		RIGHT = 3,
		ACTION = 4,
		CONFIRM = 5,
		CANCEL = 6,
	};
}  // namespace InputType

class Inputs {
public:
	static const int						nb_input = 7;
	static const std::string				input_type_name[Inputs::nb_input];
	static const std::string				_conf_file;

	~Inputs();

	static Inputs							&get();
	static bool								getKey(InputType::Enum type);
	static void								configureKey(InputType::Enum type);
	static void								cancelConfiguration();
	static bool								shouldQuit();
	static const glm::ivec2					&getMousePos();
	static const glm::ivec2					&getMouseRel();
	static void								update();

private:
	Inputs();
	Inputs(const Inputs &src);
	Inputs 									&operator=(const Inputs &src);

	bool									_getKey(InputType::Enum type) const;
	void									_configureKey(InputType::Enum type);
	void									_cancelConfiguration();
	bool									_shouldQuit() const;
	const glm::ivec2						&_getMousePos() const;
	const glm::ivec2						&_getMouseRel() const;
	void									_update();

	bool									_configuring;
	InputType::Enum							_next_action_type;
	bool									_quit;
	glm::ivec2								_mouse_pos;
	glm::ivec2								_mouse_rel;
	std::map<SDL_Scancode, InputType::Enum>	_input_key_map;
	std::unordered_set<int64_t>				_used_scan;
	bool									_key_status[Inputs::nb_input];
	SettingsJson							_controls;
};

#endif
