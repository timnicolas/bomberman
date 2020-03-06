#include "ABaseUI.hpp"
#include "Logging.hpp"
#include "debug.hpp"
#include "Texture.hpp"

/*
	draw a rect at `pos` of size `size` with the color `color`
*/
void ABaseUI::_drawRect(glm::vec2 pos, glm::vec2 size, glm::vec4 color1, glm::vec4 color2, float factor) {
	_rectShader->use();

	// set color
    _rectShader->setVec4("masterColor", color1);  // set master color
    _rectShader->setVec4("secondColor", color2);  // set secondary color
    _rectShader->setFloat("colorFactor", factor);  // set factor

	// set model matrix
	glm::mat4 model = glm::mat4(1.0);
	model = glm::translate(model, glm::vec3(pos.x, pos.y, 0));
	model = glm::scale(model, glm::vec3(size.x, size.y, 0));
	_rectShader->setMat4("model", model);

	// draw triangles
	glBindVertexArray(_rectVao);
	glDrawArrays(GL_TRIANGLES, 0, 6);  // draw
	glBindVertexArray(0);

    _rectShader->unuse();
}
/*
	draw the border of a rect at `pos` of size `size` with the color `color`
*/
void ABaseUI::_drawBorderRect(glm::vec2 pos, glm::vec2 size, float borderSize, glm::vec4 color) {
	if (borderSize == 0)
		return;
	glm::vec2 tmpPos;
	glm::vec2 tmpSize = size;
	tmpSize.y = borderSize;

	tmpPos = pos;
	_drawRect(tmpPos, tmpSize, color);
	tmpPos = pos;
	tmpPos.y += size.y - borderSize;
	_drawRect(tmpPos, tmpSize, color);

	tmpSize = size;
	tmpSize.x = borderSize;

	tmpPos = pos;
	_drawRect(tmpPos, tmpSize, color);
	tmpPos = pos;
	tmpPos.x += size.x - borderSize;
	_drawRect(tmpPos, tmpSize, color);
}

/*
	draw text centered on a pos
		pos -> position of the center of the text
		scale -> size of the text
		text -> the text to print
*/
void ABaseUI::_drawText(glm::vec2 pos, glm::vec2 size, std::string const & font, float scale, std::string const & text,
glm::vec4 color, TextAlign::Enum align, float padding) {
	(void)align;
	uint32_t width = _textRender->strWidth(font, text, scale);
	uint32_t height = _textRender->strHeight(font, text, scale);

	// get position of the text
	glm::vec2 tmpPos;
	if (align == TextAlign::LEFT)
		tmpPos.x = pos.x + padding;
	else if (align == TextAlign::CENTER)
		tmpPos.x = (pos.x + size.x / 2) - width / 2;
	else if (align == TextAlign::RIGHT)
		tmpPos.x = pos.x + size.x - width - padding;
	tmpPos.y = (pos.y + size.y / 2) - height / 2;
	_textRender->write(font, text, tmpPos.x, tmpPos.y, scale, color);
}

/*
	create a texture from an image
	if updateSize is set to true:
		- if _size.x == 0 && _size.x == 0 -> size auto setted (default size for the image)
		- if _size.x == 0 -> width auto setted
		- if _size.y == 0 -> height auto setted

	@return: the texture ID
*/
void ABaseUI::_loadImg(std::string const & filename, bool updateSize, bool pixelateOnZoom) {
	// load the texture
	try {
		_imgTextureID = textureFromFile(filename, true, pixelateOnZoom, &_imgDefWidth, &_imgDefHeight);
	}
	catch (TextureFailToLoad & e) {
		throw UIException(e.what());
	}

	// auto set size if needed
	if (updateSize) {
		if (_size.x == 0 && _size.y == 0) {
			_size.x = _imgDefWidth;
			_size.y = _imgDefHeight;
		}
		else if (_size.x == 0) {
			_size.x = _size.y * (static_cast<float>(_imgDefWidth) / _imgDefHeight);
		}
		else if (_size.y == 0) {
			_size.y = _size.x * (static_cast<float>(_imgDefHeight) / _imgDefWidth);
		}
	}
}
/*
	draw a 2D image on the screen
*/
void ABaseUI::_drawImg(glm::vec2 pos, glm::vec2 size, GLuint textureID, glm::vec4 color) {
	_imgShader->use();

	// send color
    _imgShader->setVec4("color", color);

	// set model matrix
	glm::mat4 model = glm::mat4(1.0);
	model = glm::translate(model, glm::vec3(pos.x, pos.y, 0));
	model = glm::scale(model, glm::vec3(size.x, size.y, 0));
	_imgShader->setMat4("model", model);

	// send texture
    glBindVertexArray(_imgVao);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureID);
	// draw image
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindTexture(GL_TEXTURE_2D, 0);
    glBindVertexArray(0);

    _imgShader->unuse();
}
