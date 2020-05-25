#include "TextRender.hpp"
#include "debug.hpp"
#include "Logging.hpp"

/**
 * @brief Construct a new Text Render:: Text Render object
 *
 * @param width The screen width
 * @param height The screen height
 */
TextRender::TextRender(uint32_t width, uint32_t height) :
_shader(SHADER_TEXT_VS, SHADER_TEXT_FS)
{
	setWinSize(glm::vec2(width, height));
	// create VAO & VBO
	_vao = 0;
	_vbo = 0;
	_shader.use();
	glGenVertexArrays(1, &_vao);
	glGenBuffers(1, &_vbo);
	glBindVertexArray(_vao);
	glBindBuffer(GL_ARRAY_BUFFER, _vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * SHADER_TEXT_ROW_SIZE, NULL, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, SHADER_TEXT_ROW_SIZE * sizeof(GLfloat), 0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, SHADER_TEXT_ROW_SIZE * sizeof(GLfloat),
		reinterpret_cast<void*>(3 * sizeof(float)));
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	_shader.unuse();
}

/**
 * @brief Load a ttf file
 *
 * @param name The name of the font created (used later to draw)
 * @param filename The ttf filename
 * @param size The text default size
 */
void TextRender::loadFont(std::string name, std::string const &filename, uint32_t size) {
	FT_Library ft;
	if (FT_Init_FreeType(&ft)) {
		logErr("Could not init FreeType Library");
		throw TextRender::LoadTextRenderException();
	}
	FT_Face face;
	if (FT_New_Face(ft, filename.c_str(), 0, &face)) {
		logErr("Failed to load font: " << filename);
		FT_Done_FreeType(ft);
		throw TextRender::LoadTextRenderException();
	}
	FT_Set_Pixel_Sizes(face, 0, size);  // set size

	font.insert(std::pair<std::string, std::map<GLchar, Character> >(name, std::map<GLchar, Character>()));


	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	for (GLubyte c = 0; c < 128; c++) {
		// load char
		if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
			logErr("Failed to load char: " << c << " in " << filename);
			continue;
		}
		// generate texture
		GLuint texture;
		glGenTextures(1, &texture);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, face->glyph->bitmap.width, face->glyph->bitmap.rows, 0,
			GL_RED, GL_UNSIGNED_BYTE, face->glyph->bitmap.buffer);
		// set textures options
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		// save char args for future write
		Character character = {
			texture,
			glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
			glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
			face->glyph->advance.x
		};
		font[name].insert(std::pair<GLchar, Character>(c, character));
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	// delete freetype objects
	FT_Done_Face(face);
	FT_Done_FreeType(ft);
}

/**
 * @brief Construct a new Text Render:: Text Render object
 *
 * @param src The object to do the copy
 */
TextRender::TextRender(TextRender const &src) :
_shader(src.getShader()) {
	*this = src;
}

/**
 * @brief Destroy the Text Render:: Text Render object
 */
TextRender::~TextRender() {
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDeleteVertexArrays(1, &_vao);
	glDeleteBuffers(1, &_vbo);

	for (auto const & elem : font) {
		if (elem.second.size() > 0 && elem.second.begin()->second.textureID != 0) {
			glDeleteTextures(1, &elem.second.begin()->second.textureID);
		}
	}
	_shader.unuse();
}

/**
 * @brief Copy this object
 *
 * @param rhs The object to copy
 * @return TextRender& A reference to the copied object
 */
TextRender &TextRender::operator=(TextRender const &rhs) {
	(void)rhs;
	if (this != &rhs) {
		_vao = rhs._vao;
		_vbo = rhs._vbo;
	}
	return *this;
}

/**
 * @brief Call this function when the window is resized
 *
 * @param winSize The new window size
 */
void TextRender::setWinSize(glm::vec2 winSize) {
	_projection = glm::ortho(
		0.0f,
		static_cast<GLfloat>(winSize.x),
		0.0f,
		static_cast<GLfloat>(winSize.y));
	_shader.use();
	_shader.setMat4("projection", _projection);
	_shader.unuse();
}

/**
 * @brief Write 2D text on screen
 *
 * @param fontName The name of the font (choose name when load font)
 * @param text The text to write
 * @param pos The text position
 * @param scale The text scale
 * @param color The text color
 * @param outline The outline size
 * @param colorOutline The color of outline
 */
void TextRender::write(std::string const &fontName, std::string text, glm::vec3 pos,
GLfloat scale, glm::vec3 color, int outline, glm::vec3 colorOutline) {
	if (font.find(fontName) == font.end()) {
		logErr("invalid font name " << fontName);
		return;
	}
	_shader.use();
	_shader.setVec3("textColor", color);
	glActiveTexture(GL_TEXTURE0);
	glBindVertexArray(_vao);
	glm::vec3 savePos = pos;
	for (auto c = text.begin(); c != text.end(); c++) {  // foreach chars
		Character ch = font[fontName][*c];
		_writeChar(ch, pos, scale);
		// move cursor to the next character
		pos.x += (ch.advance >> 6) * scale;
	}
	_shader.setVec3("textColor", colorOutline);
	if (outline > 0) {
		pos = savePos;
		for (auto c = text.begin(); c != text.end(); c++) {  // foreach chars
			Character ch = font[fontName][*c];
			_writeChar(ch, {pos.x + outline, pos.y + outline, pos.z}, scale);
			_writeChar(ch, {pos.x + outline, pos.y, pos.z}, scale);
			_writeChar(ch, {pos.x + outline, pos.y - outline, pos.z}, scale);
			_writeChar(ch, {pos.x, pos.y - outline, pos.z}, scale);
			_writeChar(ch, {pos.x, pos.y + outline, pos.z}, scale);
			_writeChar(ch, {pos.x - outline, pos.y + outline, pos.z}, scale);
			_writeChar(ch, {pos.x - outline, pos.y, pos.z}, scale);
			_writeChar(ch, {pos.x - outline, pos.y - outline, pos.z}, scale);
			// move cursor to the next character
			pos.x += (ch.advance >> 6) * scale;
		}
	}
	glBindVertexArray(0);
	_shader.unuse();
}

/**
 * @brief Write 2D character on screen
 *
 * @param c The charater to write
 * @param fontName The name of the font (choose name when load font)
 * @param pos The character position
 * @param scale The character scale
 * @param color The characater color
 */
void TextRender::_writeChar(Character ch, glm::vec3 pos, GLfloat scale) {
	GLfloat xpos = pos.x + ch.bearing.x * scale;
	GLfloat ypos = pos.y - (ch.size.y - ch.bearing.y) * scale;
	GLfloat w = ch.size.x * scale;
	GLfloat h = ch.size.y * scale;
	// set VBO values
	GLfloat vertices[6][SHADER_TEXT_ROW_SIZE] = {
		{xpos,     ypos + h, pos.z,   0.0, 0.0},
		{xpos,     ypos,     pos.z,   0.0, 1.0},
		{xpos + w, ypos,     pos.z,   1.0, 1.0},
		{xpos,     ypos + h, pos.z,   0.0, 0.0},
		{xpos + w, ypos,     pos.z,   1.0, 1.0},
		{xpos + w, ypos + h, pos.z,   1.0, 0.0},
		};
	// bind char texture
	glBindTexture(GL_TEXTURE_2D, ch.textureID);
	// set VBO on shader
	glBindBuffer(GL_ARRAY_BUFFER, _vbo);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	// draw char
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindTexture(GL_TEXTURE_2D, 0);
}

/**
 * @brief Get the width of a text (in pixel) with a given font and size
 *
 * @param fontName The name of the font (choose name when load font)
 * @param text The text to write
 * @param scale The text scale
 * @return uint32_t The text width
 */
uint32_t	TextRender::strWidth(std::string const &fontName, std::string text, GLfloat scale) {
	uint32_t	width = 0;
	if (font.find(fontName) == font.end()) {
		logErr("invalid font name " << fontName);
		return 0;
	}
	for (auto c = text.begin(); c != text.end(); c++) {  // foreach chars
		Character ch = font[fontName][*c];
		width += (ch.advance >> 6) * scale;
	}
	return width;
}

/**
 * @brief Get the height of a text (in pixel) with a given font and size
 *
 * @param fontName The name of the font (choose name when load font)
 * @param scale The text scale
 * @param fullHeight True if we need the full height (including the letter under base height like 'j')
 * @return uint32_t The text height
 */
uint32_t	TextRender::strHeight(std::string const &fontName, GLfloat scale, bool fullHeight) {
	std::string text;  // This if the text to check (L to have only positive height, jL to have full height)
	if (fullHeight)
		 text = "j|Ll1";
	else
		text = "Ll1";
	uint32_t	height = 0;
	if (font.find(fontName) == font.end()) {
		logErr("invalid font name " << fontName);
		return 0;
	}
	for (auto c = text.begin(); c != text.end(); c++) {  // foreach chars
		Character ch = font[fontName][*c];
		uint32_t tmpH = ch.size.y * scale;
		if (tmpH > height) {
			height = tmpH;
		}
	}
	return height;
}

/**
 * @brief Get the shader
 *
 * @return Shader& Shader reference
 */
Shader			&TextRender::getShader() { return _shader; }
/**
 * @brief Get the shader
 *
 * @return Shader& Shader reference
 */
Shader const	&TextRender::getShader() const { return _shader; }
