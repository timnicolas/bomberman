#pragma once

#include "includesOpengl.hpp"
#include "ABaseUI.hpp"

class ButtonImageUI : public ABaseUI {
	public:
		ButtonImageUI(glm::vec2 pos, glm::vec2 size, std::string const & filename,
			bool pixelateOnZoom = true);
		ButtonImageUI(ButtonImageUI const & src);
		virtual ~ButtonImageUI();

		ButtonImageUI & operator=(ButtonImageUI const & rhs);

		virtual void	_draw();

	protected:
		virtual void	_update(glm::vec2 mousePos, bool rightClick, bool leftClick);
		ButtonImageUI();
};
