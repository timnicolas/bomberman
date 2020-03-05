#include "ImageUI.hpp"
#include "Logging.hpp"
#include "debug.hpp"
#include "Texture.hpp"

ImageUI::ImageUI(glm::vec2 winSize, glm::vec2 pos, glm::vec2 size, std::string const & filename)
: ABaseUI(winSize, pos, size),
  _filename(filename),
  _imgShader(SHADER_IMAGE_2D_VS, SHADER_IMAGE_2D_FS)
{
	_color = glm::vec4(0.0, 0.0, 0.0, 0.0);
	// create VAO & VBO
	_vao = 0;
	_vbo = 0;
	_imgShader.use();
	glGenVertexArrays(1, &_vao);
	glGenBuffers(1, &_vbo);
	glBindVertexArray(_vao);
	glBindBuffer(GL_ARRAY_BUFFER, _vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * SHADER_IMAGE_2D_ROW_SIZE, NULL, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, SHADER_IMAGE_2D_ROW_SIZE * sizeof(float), reinterpret_cast<void*>(0));
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, SHADER_IMAGE_2D_ROW_SIZE * sizeof(float),
							reinterpret_cast<void*>(2 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	try {
		_imgShader.setMat4("projection", _projection);
	}
	catch (TextureFailToLoad & e) {
		throw UIException(e.what());
	}

	int w, h;
	_textureID = textureFromFile(filename, true, &w, &h);
	if (_size.x == 0 && _size.y == 0) {
		_size.x = w;
		_size.y = h;
	}
	else if (_size.x == 0) {
		_size.x = _size.y * (static_cast<float>(w) / h);
	}
	else if (_size.y == 0) {
		_size.y = _size.x * (static_cast<float>(h) / w);
	}

	_imgShader.unuse();
}

ImageUI::ImageUI(ImageUI const & src)
: ABaseUI(src),
  _imgShader(SHADER_IMAGE_2D_VS, SHADER_IMAGE_2D_FS)
{
	*this = src;
}

ImageUI::~ImageUI() {
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDeleteVertexArrays(1, &_vao);
	glDeleteBuffers(1, &_vbo);
}

ImageUI & ImageUI::operator=(ImageUI const & rhs) {
	if (this != &rhs) {
		_vao = rhs._vao;
		_vbo = rhs._vbo;
	}
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
	_drawImg(_pos, _size, _textureID, _color);
}

/*
	draw a 2D image on the screen
*/
void ImageUI::_drawImg(glm::vec2 pos, glm::vec2 size, GLuint textureID, glm::vec4 color) {
	_imgShader.use();
    _imgShader.setVec4("color", color);
    glBindVertexArray(_vao);
	// set VBO values
	GLfloat vertices[6][SHADER_IMAGE_2D_ROW_SIZE] = {
		{pos.x,          pos.y + size.y, 0.0, 0.0},
		{pos.x,          pos.y,          0.0, 1.0},
		{pos.x + size.x, pos.y,          1.0, 1.0},
		{pos.x,          pos.y + size.y, 0.0, 0.0},
		{pos.x + size.x, pos.y,          1.0, 1.0},
		{pos.x + size.x, pos.y + size.y, 1.0, 0.0},
	};
	// set VBO on shader
	glBindBuffer(GL_ARRAY_BUFFER, _vbo);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	// draw image
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindTexture(GL_TEXTURE_2D, 0);
    glBindVertexArray(0);
    _imgShader.unuse();
}
