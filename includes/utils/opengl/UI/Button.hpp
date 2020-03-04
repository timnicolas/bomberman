#pragma once

#include "includesOpengl.hpp"
#include "ABaseUI.hpp"

class Button : public ABaseUI {
	public:
		Button(glm::vec2 winSize, glm::vec2 pos, glm::vec2 size);
		Button(Button const & src);
		virtual ~Button();

		Button & operator=(Button const & rhs);

		virtual void	draw();

	protected:
		virtual void	_update(glm::vec2 mousePos, MouseState::ENUM mouseState);
		Button();
};
