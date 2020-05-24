#pragma once

#include "includesOpengl.hpp"
#include "ABaseUI.hpp"

/**
 * @brief this is the UI for button image
 */
class ButtonImageUI : public ABaseUI {
	public:
		ButtonImageUI(glm::vec2 pos, glm::vec2 size, std::string const & filename,
			std::string const & filenameHover = "");
		ButtonImageUI(ButtonImageUI const & src);
		virtual ~ButtonImageUI();

		ButtonImageUI & operator=(ButtonImageUI const & rhs);

	protected:
		virtual void	_update();
		virtual void	_draw();
		ButtonImageUI();
};
