#include "ScrollbarUI.hpp"
#include "Logging.hpp"
#include "debug.hpp"

ScrollbarUI::ScrollbarUI(glm::vec2 pos, glm::vec2 size)
: ABaseMasterUI(pos, size),
  _scrollbarSize(20),
  _scrollbarColor(0.3, 0.3, 0.3, 1.0),
  _vertScroll(false),
  _vertScrollbarPos(0),
  _vertScrollBarDrawSize(0),
  _horizScroll(false),
  _horizScrollbarPos(0)
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
	/* update master size */
	_updateTotalMasterSize();

	/* get mouse position */
	glm::vec2 mousePos = Inputs::getMousePos();
	mousePos.y = _winSize.y - mousePos.y;

	if (_vertScroll) {
		/* size of the slider */
		float scrollSizeFactor = getMasterSize().y / _masterTotalSize.y;
		if (scrollSizeFactor < 0.1) scrollSizeFactor = 0.1;
		if (scrollSizeFactor > 1) scrollSizeFactor = 1;
		_vertScrollBarDrawSize = (_size.y - _borderSize * 2) * scrollSizeFactor;

		if (_leftClick) {
			if (mousePos.x >= getRealPos().x + _size.x - _borderSize - _scrollbarSize
			&& mousePos.x <= getRealPos().x + _size.x - _borderSize
			&& mousePos.y >= getRealPos().y + _borderSize
			&& mousePos.y <= getRealPos().y + _size.y - _borderSize)
			{
				// if click on the right zone
				float yMin = getRealPos().y + _borderSize;
				float yMax = getRealPos().y + _size.y - (_borderSize * 2);
				// float yMin = getRealPos().y + _borderSize + _vertScrollBarDrawSize / 2;
				// float yMax = getRealPos().y + _size.y - (_borderSize * 2 + _vertScrollBarDrawSize / 2);
				if (mousePos.y < yMin)
					_vertScrollbarPos = 1;
				else if (mousePos.y > yMax)
					_vertScrollbarPos = 0;
				else
					_vertScrollbarPos = 1 - (mousePos.y - yMin) / (yMax - yMin);
			}
		}
		_masterOffset.y = (_masterTotalSize.y - (_size.y - _borderSize * 2)) * _vertScrollbarPos;
	}
}

/**
 * @brief this is the draw function for UI
 * /!\ -> you need to draw in the reverse order (draw at first the element on the top)
 */
void ScrollbarUI::_draw() {
	glm::vec2 tmpPos;
	glm::vec2 tmpSize;

	// draw scrollbars
	if (_vertScroll) {
		/* size of the scrollbar */
		tmpSize.x = _scrollbarSize;
		tmpSize.y = _vertScrollBarDrawSize;  // height depend to the total master size

		/* position of the scrollbar */
		tmpPos.x = getRealPos().x + _size.x - _borderSize - tmpSize.x;

		float y = (1 - _vertScrollbarPos) * (_size.y - _borderSize * 2);
		y -= tmpSize.y / 2;
		if (y < 0) y = 0;
		if (y > (_size.y - _borderSize * 2) - tmpSize.y) y = (_size.y - _borderSize * 2) - tmpSize.y;

		tmpPos.x = getRealPos().x + _size.x - _borderSize - _scrollbarSize;
		tmpPos.y = getRealPos().y + _borderSize + y;

		/* draw scrollbar */
		_drawRect(tmpPos, tmpSize, _scrollbarColor);
	}

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
ScrollbarUI &	ScrollbarUI::setScrollBarSize(float size) { _scrollbarSize = size; return *this; }

/**
 * @brief Get size of master element inside the borders
 *
 * !!! This functions can be overwirte in childs class
 *
 * @return glm::vec2 The size
 */
glm::vec2 ScrollbarUI::getMasterSize() const {
	glm::vec2 size = _size;
	size.x -= 2 * _borderSize;
	if (_vertScroll)
		size.x -= _scrollbarSize;
	size.y -= 2 * _borderSize;
	if (_horizScroll)
		size.y -= _scrollbarSize;
	return size;
}
