#include "TextUI.hpp"
#include "Logging.hpp"
#include "debug.hpp"

TextUI::TextUI(glm::vec2 pos, glm::vec2 size): ABaseUI(pos, size) {
	setColor(glm::vec4(0.0, 0.0, 0.0, 0.0));
	setBorderColor(glm::vec4(0.0, 0.0, 0.0, 0.0));
}

TextUI::TextUI(TextUI const & src): ABaseUI(src) {
	*this = src;
}

TextUI::~TextUI() {}

TextUI & TextUI::operator=(TextUI const & rhs) {
	(void)rhs;
	// if (this != &rhs) {}
	return *this;
}

void TextUI::_update(glm::vec2 mousePos, bool rightClick, bool leftClick) {
	(void)mousePos;
	(void)rightClick;
	(void)leftClick;
}

/*
	this is the draw function for UI
	/!\ -> you need to draw in the reverse order (draw at first the element on the top)
*/
void TextUI::draw() {
	glm::vec2 tmpPos;
	glm::vec2 tmpSize;

	// draw text
	tmpPos = _pos;
	tmpPos.x += _borderSize;
	tmpSize = _size;
	tmpSize.x -= _borderSize * 2;
	_drawText(tmpPos, tmpSize, _textFont, _textScale, _text, _textColor, _textAlign, _textPadding);
}
