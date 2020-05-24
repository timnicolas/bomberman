#include "ButtonImageUI.hpp"
#include "Logging.hpp"
#include "debug.hpp"
#include "Texture.hpp"

/**
 * @brief Construct a new Button Image U I:: Button Image U I object
 *
 * @param pos The position of the UI element
 * @param size The size of the UI element
 * @param filename The image file
 * @param filenameHover The image file for hover mouse
 */
ButtonImageUI::ButtonImageUI(glm::vec2 pos, glm::vec2 size,
	std::string const & filename, std::string const & filenameHover)
: ABaseUI(pos, size)
{
	// disable color
	_color = glm::vec4(0.0, 0.0, 0.0, 0.0);
	_borderSize = 0;

	// load the image
	_loadImg(filename, true);
	if (filenameHover.size()) {
		_loadImg(filenameHover, false, true);
	}
}

/**
 * @brief Construct a new Button Image U I:: Button Image U I object
 *
 * @param src The object to do the copy
 */
ButtonImageUI::ButtonImageUI(ButtonImageUI const & src): ABaseUI(src) {
	*this = src;
}

/**
 * @brief Destroy the Button Image U I:: Button Image U I object
 */
ButtonImageUI::~ButtonImageUI() {
	_unloadImg();
}

/**
 * @brief Copy this object
 *
 * @param rhs The object to copy
 * @return ButtonImageUI& A reference to the copied object
 */
ButtonImageUI & ButtonImageUI::operator=(ButtonImageUI const & rhs) {
	(void)rhs;
	// if (this != &rhs) {}
	return *this;
}

/**
 * @brief this is the base update function of UI objects
 */
void ButtonImageUI::_update() {
}

/**
 * @brief this is the draw function for UI
 * /!\ -> you need to draw in the reverse order (draw at first the element on the top)
 */
void ButtonImageUI::_draw() {
	glm::vec2 tmpPos;
	glm::vec2 tmpSize;

	// draw border
	_drawBorderRect(getRealPos(), _size, _z, _borderSize, _borderColor);

	// get color (if mouse hover or mouse click)
	glm::vec4 color = _color;
	if (_imgHoverTextureID == 0) {
		if (_leftClick) {
			color = _mouseClickColor;
		}
		else if (_mouseHover) {
			color = _mouseHoverColor;
		}
	}

	// get center size and position
	tmpPos = getRealPos();
	tmpPos.x += _borderSize;
	tmpPos.y += _borderSize;
	tmpSize = _size;
	tmpSize.x -= _borderSize * 2;
	tmpSize.y -= _borderSize * 2;
	// draw image
	if ((_leftClick || _mouseHover) && _imgHoverTextureID != 0) {
		_drawImg(tmpPos, tmpSize, _z, _imgHoverTextureID, color);
	} else
	_drawImg(tmpPos, tmpSize, _z, _imgTextureID, color);
}
