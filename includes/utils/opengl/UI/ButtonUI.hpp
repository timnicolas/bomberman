#pragma once

#include "includesOpengl.hpp"
#include "ABaseUI.hpp"

class ButtonUI : public ABaseUI {
	public:
		ButtonUI(glm::vec2 pos, glm::vec2 size);
		ButtonUI(ButtonUI const & src);
		virtual ~ButtonUI();

		ButtonUI & operator=(ButtonUI const & rhs);

		virtual void	draw();

	protected:
		virtual void	_update(glm::vec2 mousePos, bool rightClick, bool leftClick);
		ButtonUI();
};
