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

		ScrollbarUI &		operator=(ScrollbarUI const & rhs);

		/* setter */
		ScrollbarUI &		enableVertScroll(bool enable);
		ScrollbarUI &		enableHorizScroll(bool enable);

		ScrollbarUI &		setScrollBarSize(float size);

		/* overwrite functions */
		virtual glm::vec2	getMasterSize() const;

	protected:
		virtual void	_update();
		virtual void	_draw();
		ScrollbarUI();

		/* scrollbar info */
		float		_scrollbarSize;
		glm::vec4	_scrollbarColor;

		// vertical
		bool		_vertScroll;
		float		_vertScrollbarPos;
		float		_vertScrollBarDrawSize;

		// horizontal
		bool		_horizScroll;
		float		_horizScrollbarPos;
};
