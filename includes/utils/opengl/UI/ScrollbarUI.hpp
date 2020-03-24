#pragma once

#include "includesOpengl.hpp"
#include "ABaseUI.hpp"
#include "ABaseMasterUI.hpp"

/**
 * @brief this is the UI for scrollbar
 */
class ScrollbarUI : public ABaseMasterUI {
	public:
		ScrollbarUI(glm::vec2 pos, glm::vec2 size);
		ScrollbarUI(ScrollbarUI const & src);
		virtual ~ScrollbarUI();

		ScrollbarUI &	operator=(ScrollbarUI const & rhs);

		/* setter */
		ScrollbarUI &	enableVertScroll(bool enable);
		ScrollbarUI &	enableHorizScroll(bool enable);

	protected:
		virtual void	_update();
		virtual void	_draw();
		ScrollbarUI();

		bool	_vertScroll;
		bool	_horizScroll;
};
