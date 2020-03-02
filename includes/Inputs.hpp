#ifndef INPUTS_HPP
# define INPUTS_HPP

# include <SDL2/SDL.h>
# include <map>
# include <array>
# include <glm/glm.hpp>
# include "SettingsJson.hpp"

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
}			// namespace InputType

class Inputs {
public:
	static const int						nb_input = 7;

	Inputs();
	~Inputs();

	bool				getKey(InputType::Enum type);
	void				configureKeys();
	bool				shouldQuit();
	const glm::ivec2	&getMousePos();
	const glm::ivec2	&getMouseRel();
	void				update();

private:
	static const std::string				_conf_file;

	Inputs(const Inputs &src);
	Inputs 			&operator=(const Inputs &src);

	bool									_quit;
	glm::ivec2								_mouse_pos;
	glm::ivec2								_mouse_rel;
	std::map<SDL_Scancode, InputType::Enum>	_input_key_map;
	bool									_key_status[Inputs::nb_input];
	SettingsJson							_controls;
};

#endif
