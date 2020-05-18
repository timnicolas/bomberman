#include "ImageAtlasRender.hpp"
#include "debug.hpp"
#include "Logging.hpp"

/**
 * @brief Construct a new Image Atlas Render:: Image Atlas Render object
 *
 * @param textureManager TextureManager object
 * @param width Image width
 * @param height Image Height
 */
ImageAtlasRender::ImageAtlasRender(TextureManager const &textureManager, uint32_t width, uint32_t height) :
_textureManager(textureManager), _shader(SHADER_IMAGE_VS, SHADER_IMAGE_FS),
_projection(glm::ortho(0.0f, static_cast<GLfloat>(width), 0.0f, static_cast<GLfloat>(height))) {
	// create VAO & VBO
	_vao = 0;
	_vbo = 0;
	_shader.use();
	glGenVertexArrays(1, &_vao);
	glGenBuffers(1, &_vbo);
	glBindVertexArray(_vao);
	glBindBuffer(GL_ARRAY_BUFFER, _vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * SHADER_IMAGE_ROW_SIZE, NULL, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, SHADER_IMAGE_ROW_SIZE * sizeof(float), reinterpret_cast<void*>(0));
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, SHADER_IMAGE_ROW_SIZE * sizeof(float),
							reinterpret_cast<void*>(2 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, SHADER_IMAGE_ROW_SIZE * sizeof(float),
							reinterpret_cast<void*>(4 * sizeof(float)));
	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	_shader.setMat4("projection", _projection);
	_shader.unuse();
}

/**
 * @brief Construct a new Image Atlas Render:: Image Atlas Render object
 *
 * @param src The object to do the copy
 */
ImageAtlasRender::ImageAtlasRender(ImageAtlasRender const &src) :
_textureManager(src._textureManager), _shader(src.getShader()) {
	*this = src;
}

/**
 * @brief Destroy the Image Atlas Render:: Image Atlas Render object
 */
ImageAtlasRender::~ImageAtlasRender() {
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDeleteVertexArrays(1, &_vao);
	glDeleteBuffers(1, &_vbo);
}

/**
 * @brief Copy this object
 *
 * @param rhs The opjet to copy
 * @return ImageAtlasRender& A reference to the copied object
 */
ImageAtlasRender &ImageAtlasRender::operator=(ImageAtlasRender const &rhs) {
	(void)rhs;
	if (this != &rhs) {
		_vao = rhs._vao;
		_vbo = rhs._vbo;
	}
	return *this;
}

/**
 * @brief Draw an image on the screen
 *
 * @param pos Position of the image
 * @param size Size of the image
 * @param texID Texture ID of the image
 * @param color Color of the image
 */
void ImageAtlasRender::draw(glm::vec2 pos, glm::vec2 size, int texID, glm::vec4 color) {
	draw(pos.x, pos.y, size.x, size.y, texID, color);
}
/**
 * @brief Draw an image on the screen
 *
 * @param posx Position x of the image
 * @param posy Position y of the image
 * @param w Width of the image
 * @param h Height of the image
 * @param texID Texture ID of the image
 * @param color Color of the image
 */
void ImageAtlasRender::draw(float posx, float posy, float w, float h, int texID, glm::vec4 color) {
    _shader.use();
    _shader.setVec4("color", color);
    glBindVertexArray(_vao);
	// set VBO values
	GLfloat vertices[6][SHADER_IMAGE_ROW_SIZE] = {
		{posx,     posy + h,   0.0, 0.0, static_cast<float>(texID)},
		{posx,     posy,       0.0, 1.0, static_cast<float>(texID)},
		{posx + w, posy,       1.0, 1.0, static_cast<float>(texID)},
		{posx,     posy + h,   0.0, 0.0, static_cast<float>(texID)},
		{posx + w, posy,       1.0, 1.0, static_cast<float>(texID)},
		{posx + w, posy + h,   1.0, 0.0, static_cast<float>(texID)},
	};
	// set VBO on shader
	glBindBuffer(GL_ARRAY_BUFFER, _vbo);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	_textureManager.activateTextures();
	// draw image
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
    glBindVertexArray(0);
    _shader.unuse();
}

/**
 * @brief Get the shader
 *
 * @return Shader& The shader
 */
Shader			&ImageAtlasRender::getShader() { return _shader; }
/**
 * @brief Get the shader
 *
 * @return Shader& The shader
 */
Shader const	&ImageAtlasRender::getShader() const { return _shader; }
