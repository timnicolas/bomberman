#pragma once

#include "includesOpengl.hpp"
#include "ABaseUI.hpp"

/**
 * @brief this is the UI for text
 */
class TextUI : public ABaseUI {
	public:
		TextUI(glm::vec2 pos, glm::vec2 size);
		TextUI(TextUI const & src);
		virtual ~TextUI();

		TextUI & operator=(TextUI const & rhs);

	protected:
		virtual void	_update();
		virtual void	_draw();
		TextUI();
};
