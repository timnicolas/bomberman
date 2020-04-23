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

		ScrollbarUI &		setScrollbarSize(float size);
		ScrollbarUI &		setScrollbarSpeed(float speed);
		ScrollbarUI &		invertVertScroll(bool invert);
		ScrollbarUI &		invertHorizScroll(bool invert);

		/* overwrite functions */
		virtual glm::vec2	getMasterPos() const;
		virtual glm::vec2	getMasterRealPos() const;
		virtual glm::vec2	getMasterSize() const;

	protected:
		virtual void	_update();
		virtual void	_draw();
		ScrollbarUI();

		/* scrollbar info */
		float		_scrollbarSize;
		glm::vec4	_scrollbarColor;
		float		_mouseScrollSpeed;

		// vertical
		bool		_vertScroll;
		bool		_vertScrollHide;  // hide because his size is 100%
		bool		_vertScrollInverted;
		float		_vertScrollbarPos;
		float		_vertScrollBarDrawSize;
		bool		_isVertScrollClicked;

		// horizontal
		bool		_horizScroll;
		bool		_horizScrollHide;  // hide because his size is 100%
		bool		_horizScrollInverted;
		float		_horizScrollbarPos;
		float		_horizScrollBarDrawSize;
		bool		_isHorizScrollClicked;
};
