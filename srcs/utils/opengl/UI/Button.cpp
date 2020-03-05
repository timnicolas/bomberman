#include "Button.hpp"
#include "Logging.hpp"
#include "debug.hpp"

Button::Button(glm::vec2 winSize, glm::vec2 pos, glm::vec2 size)
: ABaseUI(winSize, pos, size)
{
}

Button::Button(Button const & src)
: ABaseUI(src)
{
	*this = src;
}

Button::~Button() {
}

Button & Button::operator=(Button const & rhs) {
	(void)rhs;
	// if (this != &rhs) {}
	return *this;
}

void Button::_update(glm::vec2 mousePos, bool rightClick, bool leftClick) {
	(void)mousePos;
	(void)rightClick;
	(void)leftClick;
}

/*
	this is the draw function for buttons
	/!\ -> you need to draw in the reverse order (draw at first the element on the top)
*/
void Button::draw() {
	glm::vec2 tmpPos;
	glm::vec2 tmpSize;

	// draw text
	tmpPos = _pos;
	tmpPos.x += _borderSize;
	tmpSize = _size;
	tmpSize.x -= _borderSize * 2;
	_drawText(tmpPos, tmpSize, _textScale, _text, _textColor, _textAlign);

	// get center size and position
	tmpPos = _pos;
	tmpPos.x += _borderSize;
	tmpPos.y += _borderSize;
	tmpSize = _size;
	tmpSize.x -= _borderSize * 2;
	tmpSize.y -= _borderSize * 2;


    // draw center
	// set color filter if mouse over or left click
	float factor = 1;
	glm::vec4 secColor = glm::vec4(0.0, 0.0, 0.0, 0.0);
	if (_leftClick) {
		secColor = _mouseClickColor;
		factor = 1 - secColor.a;
		secColor.a = 1;
	}
	else if (_mouseHover) {
		secColor = _mouseHoverColor;
		factor = 1 - secColor.a;
		secColor.a = 1;
	}
	_drawRect(tmpPos, tmpSize, _color, secColor, factor);

	// draw border
	_drawRect(_pos, _size, _borderColor);
}
