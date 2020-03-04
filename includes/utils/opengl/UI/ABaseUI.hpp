#pragma once

#include "includesOpengl.hpp"

namespace MouseState {
	enum ENUM {
		START_RIGHT_CLICK,
		START_LEFT_CLICK,
		END_RIGHT_CLICK,
		END_LEFT_CLICK,
		NO_CLICK,
	};
};

class ABaseUI {
	public:
		ABaseUI(glm::vec2 pos, glm::vec2 size);
		ABaseUI(ABaseUI const & src);
		virtual ~ABaseUI();

		ABaseUI & operator=(ABaseUI const & rhs);

		virtual void		update(glm::vec2 mousePos, MouseState::ENUM mouseState) = 0;
		virtual void		draw() = 0;

		// getter
		glm::vec2 const &	getPos() const;
		glm::vec2 &			getPos();
		glm::vec2 const &	getSize() const;
		glm::vec2 &			getSize();

	protected:
		ABaseUI();

		glm::vec2	_pos;
		glm::vec2	_size;
};
