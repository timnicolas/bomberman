#include "ABaseUI.hpp"
#include "Logging.hpp"
#include "debug.hpp"
#include "Texture.hpp"

/**
 * @brief draw a rectangle
 *
 * @param pos the rect pos
 * @param size the rect size
 * @param color1 the rect master color
 * @param color2 the rect secondary color (to make a mix)
 * @param factor the factor of second color on master color
 */
void ABaseUI::_drawRect(glm::vec2 pos, glm::vec2 size, float z, glm::vec4 color1, glm::vec4 color2, float factor) {
	_rectShader->use();

	// set color
    _rectShader->setVec4("masterColor", color1);  // set master color
    _rectShader->setVec4("secondColor", color2);  // set secondary color
    _rectShader->setFloat("colorFactor", factor);  // set factor

	// set model matrix
	glm::mat4 model = glm::mat4(1.0);
	model = glm::translate(model, glm::vec3(pos.x, pos.y, z));
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

/**
 * @brief draw the border of a rect
 *
 * @param pos the rect pos
 * @param size the rect size
 * @param borderSize the size of the border of the rect
 * @param color the rect color
 */
void ABaseUI::_drawBorderRect(glm::vec2 pos, glm::vec2 size, float z, float borderSize, glm::vec4 color) {
	if (borderSize == 0)
		return;
	glm::vec2 tmpPos;
	glm::vec2 tmpSize = size;
	tmpSize.y = borderSize;

	tmpPos = pos;
	_drawRect(tmpPos, tmpSize, z, color);
	tmpPos = pos;
	tmpPos.y += size.y - borderSize;
	_drawRect(tmpPos, tmpSize, z, color);

	tmpSize = size;
	tmpSize.x = borderSize;

	tmpPos = pos;
	_drawRect(tmpPos, tmpSize, z, color);
	tmpPos = pos;
	tmpPos.x += size.x - borderSize;
	_drawRect(tmpPos, tmpSize, z, color);
}

/**
 * @brief draw text
 *
 * @param pos the text pos
 * @param size the text size
 * @param font the font used (see loadFont)
 * @param scale the text scale
 * @param text the text str
 * @param color the text color
 * @param align the text alignment (LEFT | CENTER | RIGHT)
 * @param padding the text padding (left & right)
 */
void ABaseUI::_drawText(glm::vec2 pos, glm::vec2 size, float z, std::string const & font, float scale,
std::string const & text, glm::vec4 color, TextAlign::Enum align, float padding) {
	(void)align;
	uint32_t width = _textRender->strWidth(font, text, scale);
	uint32_t height = _textRender->strHeight(font, scale);

	// get position of the text
	glm::vec2 tmpPos;
	tmpPos.x = 0;
	if (align == TextAlign::LEFT)
		tmpPos.x = pos.x + padding;
	else if (align == TextAlign::CENTER)
		tmpPos.x = (pos.x + size.x / 2) - width / 2;
	else if (align == TextAlign::RIGHT)
		tmpPos.x = pos.x + size.x - width - padding;
	tmpPos.y = (pos.y + size.y / 2) - height / 2;
	_textRender->write(font, text, {tmpPos.x, tmpPos.y, z}, scale, glm::vec3(color.x, color.y, color.z));
}

/**
 * @brief load an image (used in constructors)
 *
 * @param filename the image filename
 * @param updateSize auto update _size variable or not (bool)
 *   - if _size.x == 0 && _size.x == 0 -> size auto setted (default size for the image)
 *   - if _size.x == 0 -> width auto setted
 *   - if _size.y == 0 -> height auto setted
 *
 * @throw UIException if the image failed to load
 */
void ABaseUI::_loadImg(std::string const & filename, bool updateSize) {
	// load the texture
	try {
		_imgTextureID = textureFromFile(filename, false, &_imgDefSize);
	}
	catch (TextureException const & e) {
		throw UIException(e.what());
	}

	// auto set size if needed
	if (updateSize) {
		if (_size.x == 0 && _size.y == 0) {
			_size.x = _imgDefSize.x;
			_size.y = _imgDefSize.y;
		}
		else if (_size.x == 0) {
			_size.x = _size.y * (static_cast<float>(_imgDefSize.x) / _imgDefSize.y);
		}
		else if (_size.y == 0) {
			_size.y = _size.x * (static_cast<float>(_imgDefSize.y) / _imgDefSize.x);
		}
	}
}

/**
 * @brief Unload a previously load image
 */
void ABaseUI::_unloadImg() {
	glDeleteTextures(1, &_imgTextureID);
}

/**
 * @brief draw a 2D image on the screen
 *
 * @param pos the text pos
 * @param size the text size
 * @param textureID the texture ID (get with _loadImg)
 * @param color an image filter color
 */
void ABaseUI::_drawImg(glm::vec2 pos, glm::vec2 size, float z, GLuint textureID, glm::vec4 color) {
	_imgShader->use();

	// send color
    _imgShader->setVec4("color", color);

	// set model matrix
	glm::mat4 model = glm::mat4(1.0);
	model = glm::translate(model, glm::vec3(pos.x, pos.y, z));
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
