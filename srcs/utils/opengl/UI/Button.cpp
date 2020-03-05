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

void Button::_update(glm::vec2 mousePos, MouseState::ENUM mouseState) {
	(void)mousePos;
	(void)mouseState;
}

/*
	this is the draw function for buttons
	/!\ -> you need to draw in the reverse order (draw at first the element on the top)
*/
void Button::draw() {
	glm::vec2 tmpPos;
	glm::vec2 tmpSize;

	// draw text
	tmpPos.x = _pos.x + _size.x / 2;
	tmpPos.y = _pos.y + _size.y / 2;
	drawTextCenter(tmpPos, _textScale, _text, _textColor);

	// get center size and position
	tmpPos = _pos;
	tmpPos.x += _borderSize;
	tmpPos.y += _borderSize;
	tmpSize = _size;
	tmpSize.x -= _borderSize * 2;
	tmpSize.y -= _borderSize * 2;

	// draw color filter if mouse over
	if (_mouseHover) {
		drawRect(tmpPos, tmpSize, _mouseHoverColor);
	}

    // draw center
	drawRect(tmpPos, tmpSize, _color);

	// draw border
	drawRect(_pos, _size, _borderColor);
}
