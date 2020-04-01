#pragma once

#include "includesOpengl.hpp"
#include "ABaseUI.hpp"

/**
 * @brief this is the UI for text input
 */
class TextInputUI : public ABaseUI {
	public:
		TextInputUI(glm::vec2 pos, glm::vec2 size);
		TextInputUI(TextInputUI const & src);
		virtual ~TextInputUI();

		TextInputUI & operator=(TextInputUI const & rhs);

		TextInputUI & setDefText(std::string const & defText);
		TextInputUI & setDefTextColor(glm::vec4 color);

	protected:
		virtual void	_update();
		virtual void	_draw();
		TextInputUI();

		/* text */
		std::string					_defText;
		glm::vec4					_defTextColor;
		/* list of keys to ignore */
		std::vector<SDL_Scancode>	_ignoredKeys;
};
