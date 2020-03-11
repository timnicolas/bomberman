#include "ButtonImageUI.hpp"
#include "Logging.hpp"
#include "debug.hpp"
#include "Texture.hpp"

ButtonImageUI::ButtonImageUI(glm::vec2 pos, glm::vec2 size,
	std::string const & filename, bool pixelateOnZoom)
: ABaseUI(pos, size)
{
	// disable color
	_color = glm::vec4(0.0, 0.0, 0.0, 0.0);
	_borderSize = 0;

	// load the image
	_loadImg(filename, true, pixelateOnZoom);
}

ButtonImageUI::ButtonImageUI(ButtonImageUI const & src): ABaseUI(src) {
	*this = src;
}

ButtonImageUI::~ButtonImageUI() {}

ButtonImageUI & ButtonImageUI::operator=(ButtonImageUI const & rhs) {
	(void)rhs;
	// if (this != &rhs) {}
	return *this;
}

/**
 * @brief this is the base update function of UI objects
 *
 */
void ButtonImageUI::_update() {
}

/**
 * @brief this is the draw function for UI
 * /!\ -> you need to draw in the reverse order (draw at first the element on the top)
 */
void ButtonImageUI::draw() {
	glm::vec2 tmpPos;
	glm::vec2 tmpSize;

	// draw border
	_drawBorderRect(getRealPos(), _size, _borderSize, _borderColor);

	// get color (if mouse hover or mouse click)
	glm::vec4 color = _color;
	if (_leftClick) {
		color = _mouseClickColor;
	}
	else if (_mouseHover) {
		color = _mouseHoverColor;
	}

	// get center size and position
	tmpPos = getRealPos();
	tmpPos.x += _borderSize;
	tmpPos.y += _borderSize;
	tmpSize = _size;
	tmpSize.x -= _borderSize * 2;
	tmpSize.y -= _borderSize * 2;
	// draw image
	_drawImg(tmpPos, tmpSize, _imgTextureID, color);
}
