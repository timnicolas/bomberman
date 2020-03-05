#include "ImageUI.hpp"
#include "Logging.hpp"
#include "debug.hpp"
#include "Texture.hpp"

ImageUI::ImageUI(glm::vec2 winSize, glm::vec2 pos, glm::vec2 size, std::string const & filename, bool pixelateOnZoom)
: ABaseUI(winSize, pos, size)
{
	// disable color
	_color = glm::vec4(0.0, 0.0, 0.0, 0.0);

	// load the image
	_loadImg(filename, true, pixelateOnZoom);
}

ImageUI::ImageUI(ImageUI const & src): ABaseUI(src) {
	*this = src;
}

ImageUI::~ImageUI() {}

ImageUI & ImageUI::operator=(ImageUI const & rhs) {
	(void)rhs;
	// if (this != &rhs) {}
	return *this;
}

void ImageUI::_update(glm::vec2 mousePos, bool rightClick, bool leftClick) {
	(void)mousePos;
	(void)rightClick;
	(void)leftClick;
}

/*
	this is the draw function for UI
	/!\ -> you need to draw in the reverse order (draw at first the element on the top)
*/
void ImageUI::draw() {
	_drawImg(_pos, _size, _imgTextureID, _color);
}
