#include "TextUI.hpp"
#include "Logging.hpp"
#include "debug.hpp"

/**
 * @brief Construct a new TextUI::TextUI object
 *
 * @param pos The position of the UI element
 * @param size The size of the UI element
 */
TextUI::TextUI(glm::vec2 pos, glm::vec2 size): ABaseUI(pos, size) {
	setColor(glm::vec4(0.0, 0.0, 0.0, 0.0));
	setBorderSize(0);
}

/**
 * @brief Construct a new TextUI::TextUI object
 *
 * @param src The object to do the copy
 */
TextUI::TextUI(TextUI const & src): ABaseUI(src) {
	*this = src;
}

/**
 * @brief Destroy the TextUI::TextUI object
 */
TextUI::~TextUI() {}

/**
 * @brief Copy this object
 *
 * @param rhs The object to copy
 * @return TextUI& A reference to the copied object
 */
TextUI & TextUI::operator=(TextUI const & rhs) {
	(void)rhs;
	// if (this != &rhs) {}
	return *this;
}

/**
 * @brief this is the base update function of UI objects
 */
void TextUI::_update() {
}

/**
 * @brief this is the draw function for UI
 * /!\ -> you need to draw in the reverse order (draw at first the element on the top)
 */
void TextUI::_draw() {
	glm::vec2 tmpPos;
	glm::vec2 tmpSize;

	// draw text
	tmpPos = getRealPos();
	tmpPos.x += _borderSize;
	tmpSize = _size;
	tmpSize.x -= _borderSize * 2;
	_drawText(tmpPos, tmpSize, _z, _textFont, _textScale, _text, _textColor, _textAlign, _textPadding);

	// get center size and position
	tmpPos = getRealPos();
	tmpPos.x += _borderSize;
	tmpPos.y += _borderSize;
	tmpSize = _size;
	tmpSize.x -= _borderSize * 2;
	tmpSize.y -= _borderSize * 2;
	_drawRect(tmpPos, tmpSize, _z, _color);

	// draw border
	_drawBorderRect(getRealPos(), _size, _z, _borderSize, _borderColor);
}
