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
		float		_scrollbarSize;  /**< Scrollbar size */
		glm::vec4	_scrollbarColor;  /**< Scrollbar color */
		float		_mouseScrollSpeed;  /**< Speed with mouse scrolling */

		/* vertical */
		bool		_vertScroll;  /**< Enable / disable vertical scroll */
		bool		_vertScrollHide;  /**< Hide because his size is 100% */
		bool		_vertScrollInverted;  /**< Invert scroll */
		float		_vertScrollbarPos;  /**< Scrollbar position */
		float		_vertScrollBarDrawSize;  /**< Scrollbar drawing size */
		bool		_isVertScrollClicked;  /**< Scrollbar clicked */

		/* horizontal */
		bool		_horizScroll;  /**< Enable / disable horizontal scroll */
		bool		_horizScrollHide;  /**< Hide because his size is 100% */
		bool		_horizScrollInverted;  /**< Invert scroll */
		float		_horizScrollbarPos;  /**< Scrollbar position */
		float		_horizScrollBarDrawSize;  /**< Scrollbar drawing size */
		bool		_isHorizScrollClicked;  /**< Scrollbar clicked */
};
