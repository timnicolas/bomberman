#pragma once

#include <chrono>
#include "includesOpengl.hpp"
#include "ABaseUI.hpp"

#define CURSOR_SPEED_MS	300

/**
 * @brief this is the UI for text input
 */
class TextInputUI : public ABaseUI {
	public:
		TextInputUI(glm::vec2 pos, glm::vec2 size);
		TextInputUI(TextInputUI const & src);
		virtual ~TextInputUI();

		TextInputUI & operator=(TextInputUI const & rhs);

		TextInputUI &		setAlwaysFocus(bool isAlwaysFocus);
		TextInputUI &		setDefText(std::string const & defText);
		TextInputUI &		setDefTextColor(glm::vec4 color);
		TextInputUI &		inputInsertText(std::string const & txt);
		TextInputUI &		inputMoveCursor(int move);
		TextInputUI &		inputDelete();
		TextInputUI &		inputSuppr();
		TextInputUI &		inputReset();
		TextInputUI &		setFocus(bool focus);
		virtual ABaseUI &	setText(std::string const & text);

		bool				hasFocus() const;

	protected:
		virtual void	_update();
		virtual void	_draw();
		TextInputUI();

		uint32_t		_getCursorOffset() const;

		/* general */
		bool						_isAlwaysFocus;  /**< TextInput has always focus or only on click */
		bool						_hasFocus;  /**< Has focus now ? */
		bool						_looseFocusNextTime;  /**< True if focus will be lost in next loop */

		/* text */
		std::string					_defText;  /**< Default text */
		glm::vec4					_defTextColor;  /**< Default text color */

		/* cursor */
		bool						_showCursor;  /**< Show cursor when typing text */
		std::chrono::milliseconds	_lastShowCursorMs;  /**< Last time cursor showed (to blink it) */
		uint32_t					_cursorPos;  /**< Cursor position */
};
