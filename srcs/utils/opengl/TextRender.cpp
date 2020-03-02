#include "TextRender.hpp"
#include "debug.hpp"
#include "Logging.hpp"

TextRender::TextRender(uint32_t width, uint32_t height) :
_shader(SHADER_TEXT_VS, SHADER_TEXT_FS),
_projection(glm::ortho(0.0f, static_cast<GLfloat>(width), 0.0f, static_cast<GLfloat>(height))) {
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
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, SHADER_TEXT_ROW_SIZE * sizeof(GLfloat), 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	_shader.setMat4("projection", _projection);
	_shader.unuse();
}

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


TextRender::TextRender(TextRender const &src) :
_shader(src.getShader()) {
	*this = src;
}

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

TextRender &TextRender::operator=(TextRender const &rhs) {
	(void)rhs;
	if (this != &rhs) {
		_vao = rhs._vao;
		_vbo = rhs._vbo;
	}
	return *this;
}

void TextRender::write(std::string const &fontName, std::string text, GLfloat x, GLfloat y,
GLfloat scale, glm::vec3 color) {
	if (font.find(fontName) == font.end()) {
		logErr("invalid font name " << fontName);
		return;
	}
    _shader.use();
    _shader.setVec3("textColor", color);
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(_vao);
	for (auto c = text.begin(); c != text.end(); c++) {  // foreach chars
        Character ch = font[fontName][*c];
        GLfloat xpos = x + ch.bearing.x * scale;
        GLfloat ypos = y - (ch.size.y - ch.bearing.y) * scale;
        GLfloat w = ch.size.x * scale;
        GLfloat h = ch.size.y * scale;
        // set VBO values
        GLfloat vertices[6][SHADER_TEXT_ROW_SIZE] = {
            {xpos,     ypos + h,   0.0, 0.0},
            {xpos,     ypos,       0.0, 1.0},
            {xpos + w, ypos,       1.0, 1.0},
            {xpos,     ypos + h,   0.0, 0.0},
            {xpos + w, ypos,       1.0, 1.0},
            {xpos + w, ypos + h,   1.0, 0.0},
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
        // move cursor to the next character
        x += (ch.advance >> 6) * scale;
    }
    glBindVertexArray(0);
    _shader.unuse();
}

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


Shader			&TextRender::getShader() { return _shader; }
Shader const	&TextRender::getShader() const { return _shader; }
