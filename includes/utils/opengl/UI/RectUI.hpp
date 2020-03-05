#pragma once

#include "includesOpengl.hpp"
#include "ABaseUI.hpp"

class RectUI : public ABaseUI {
	public:
		RectUI(glm::vec2 winSize, glm::vec2 pos, glm::vec2 size);
		RectUI(RectUI const & src);
		virtual ~RectUI();

		RectUI & operator=(RectUI const & rhs);

		virtual void	draw();

	protected:
		virtual void	_update(glm::vec2 mousePos, bool rightClick, bool leftClick);
		RectUI();
};
