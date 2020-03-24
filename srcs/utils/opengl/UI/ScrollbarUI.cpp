#include "ScrollbarUI.hpp"
#include "Logging.hpp"
#include "debug.hpp"

ScrollbarUI::ScrollbarUI(glm::vec2 pos, glm::vec2 size)
: ABaseMasterUI(pos, size),
  _vertScroll(false),
  _horizScroll(false)
{
	setColor(glm::vec4(0.0, 0.0, 0.0, 0.0));
}

ScrollbarUI::ScrollbarUI(ScrollbarUI const & src): ABaseMasterUI(src) {
	*this = src;
}

ScrollbarUI::~ScrollbarUI() {}

ScrollbarUI & ScrollbarUI::operator=(ScrollbarUI const & rhs) {
	(void)rhs;
	// if (this != &rhs) {}
	return *this;
}

/**
 * @brief this is the base update function of UI objects
 */
void ScrollbarUI::_update() {
	logInfo(glm::to_string(getTotalMasterSize()));
}

/**
 * @brief this is the draw function for UI
 * /!\ -> you need to draw in the reverse order (draw at first the element on the top)
 */
void ScrollbarUI::_draw() {
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

/* setter */
ScrollbarUI &	ScrollbarUI::enableVertScroll(bool enable) { _vertScroll = enable; return *this; }
ScrollbarUI &	ScrollbarUI::enableHorizScroll(bool enable) { _horizScroll = enable; return *this; }
