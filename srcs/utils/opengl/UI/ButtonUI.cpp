#include "ButtonUI.hpp"
#include "Logging.hpp"
#include "debug.hpp"

/**
 * @brief Construct a new Button U I:: Button U I object
 *
 * @param pos The position of the UI element
 * @param size The size of the UI element
 */
ButtonUI::ButtonUI(glm::vec2 pos, glm::vec2 size): ABaseUI(pos, size) {
}

/**
 * @brief Construct a new Button U I:: Button U I object
 *
 * @param src The object to do the copy
 */
ButtonUI::ButtonUI(ButtonUI const & src): ABaseUI(src) {
	*this = src;
}

/**
 * @brief Destroy the Button U I:: Button U I object
 */
ButtonUI::~ButtonUI() {}

/**
 * @brief Copy this object
 *
 * @param rhs The object to copy
 * @return ButtonUI& A reference to the copied object
 */
ButtonUI & ButtonUI::operator=(ButtonUI const & rhs) {
	(void)rhs;
	// if (this != &rhs) {}
	return *this;
}

/**
 * @brief this is the base update function of UI objects
 */
void ButtonUI::_update() {
}

/**
 * @brief this is the draw function for UI
 * /!\ -> you need to draw in the reverse order (draw at first the element on the top)
 */
void ButtonUI::_draw() {
	glm::vec2 tmpPos;
	glm::vec2 tmpSize;

	// draw text
	tmpPos = getRealPos();
	tmpPos.x += _borderSize;
	tmpSize = _size;
	tmpSize.x -= _borderSize * 2;
	glm::vec4 secColor = _textColor;
	if (_leftClick) {
		secColor = _mouseClickColorText;
		secColor.a = 1;
	}
	else if (_mouseHover) {
		secColor = _mouseHoverColorText;
		secColor.a = 1;
	} else if (_selected) {
		secColor = _selectedColorText;
	}
	_drawText(tmpPos, tmpSize, _z, _textFont, _textScale, _text, secColor, _textAlign, _textPadding);

	// get center size and position
	tmpPos = getRealPos();
	tmpPos.x += _borderSize;
	tmpPos.y += _borderSize;
	tmpSize = _size;
	tmpSize.x -= _borderSize * 2;
	tmpSize.y -= _borderSize * 2;


	// draw center
	// set color filter if mouse over or left click
	float factor = 1;
	secColor = glm::vec4(0.0, 0.0, 0.0, 0.0);
	if (_leftClick) {
		secColor = _mouseClickColor;
		factor = 1 - secColor.a;
		secColor.a = 1;
	}
	else if (_mouseHover) {
		secColor = _mouseHoverColor;
		factor = 1 - secColor.a;
		secColor.a = 1;
	} else if (_selected) {
		secColor = _selectedColor;
		factor = 1 - secColor.a;
		secColor.a = 1;
	}
	_drawRect(tmpPos, tmpSize, _z, _color, secColor, factor);

	// draw border
	_drawBorderRect(getRealPos(), _size, _z, _borderSize, _borderColor);
}
