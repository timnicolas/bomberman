#include "RectUI.hpp"
#include "Logging.hpp"
#include "debug.hpp"

RectUI::RectUI(glm::vec2 pos, glm::vec2 size): ABaseUI(pos, size) {
	// set the UI to non clickable
	_isClickableUI = false;
}

RectUI::RectUI(RectUI const & src): ABaseUI(src) {
	*this = src;
}

RectUI::~RectUI() {}

RectUI & RectUI::operator=(RectUI const & rhs) {
	(void)rhs;
	// if (this != &rhs) {}
	return *this;
}

/**
 * @brief this is the base update function of UI objects
 */
void RectUI::_update() {
}

/**
 * @brief this is the draw function for UI
 * /!\ -> you need to draw in the reverse order (draw at first the element on the top)
 */
void RectUI::_draw() {
	glm::vec2 tmpPos;
	glm::vec2 tmpSize;

	// get center size and position
	tmpPos = getRealPos();
	tmpPos.x += _borderSize;
	tmpPos.y += _borderSize;
	tmpSize = _size;
	tmpSize.x -= _borderSize * 2;
	tmpSize.y -= _borderSize * 2;
	_drawRect(tmpPos, tmpSize, _color);

	// draw border
	_drawBorderRect(getRealPos(), _size, _borderSize, _borderColor);
}
