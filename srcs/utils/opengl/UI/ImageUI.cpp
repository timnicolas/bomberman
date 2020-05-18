#include "ImageUI.hpp"
#include "Logging.hpp"
#include "debug.hpp"
#include "Texture.hpp"

/**
 * @brief Construct a new Image U I:: Image U I object
 *
 * @param pos The position of the UI element
 * @param size The size of the UI element
 * @param filename The image file
 */
ImageUI::ImageUI(glm::vec2 pos, glm::vec2 size, std::string const & filename)
: ABaseUI(pos, size)
{
	// set the UI to non clickable
	_isClickableUI = false;

	// disable color
	_color = glm::vec4(0.0, 0.0, 0.0, 0.0);
	_borderSize = 0;

	// load the image
	_loadImg(filename, true);
}

/**
 * @brief Construct a new Image U I:: Image U I object
 *
 * @param src The object to do the copy
 */
ImageUI::ImageUI(ImageUI const & src): ABaseUI(src) {
	*this = src;
}

/**
 * @brief Destroy the Image U I:: Image U I object
 */
ImageUI::~ImageUI() {
	_unloadImg();
}

/**
 * @brief Copy this object
 *
 * @param rhs The object to copy
 * @return ImageUI& A reference to the copied object
 */
ImageUI & ImageUI::operator=(ImageUI const & rhs) {
	(void)rhs;
	// if (this != &rhs) {}
	return *this;
}

/**
 * @brief this is the base update function of UI objects
 */
void ImageUI::_update() {
}

/**
 * @brief this is the draw function for UI
 * /!\ -> you need to draw in the reverse order (draw at first the element on the top)
 */
void ImageUI::_draw() {
	glm::vec2 tmpPos;
	glm::vec2 tmpSize;

	// draw border
	_drawBorderRect(getRealPos(), _size, _z, _borderSize, _borderColor);

	// get center size and position
	tmpPos = getRealPos();
	tmpPos.x += _borderSize;
	tmpPos.y += _borderSize;
	tmpSize = _size;
	tmpSize.x -= _borderSize * 2;
	tmpSize.y -= _borderSize * 2;
	// draw image
	_drawImg(tmpPos, tmpSize, _z, _imgTextureID, _color);
}
