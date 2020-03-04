#pragma once

#include <freetype2/ft2build.h>
#include FT_FREETYPE_H
#include <stdexcept>
#include <map>
#include "includesOpengl.hpp"
#include "Shader.hpp"

#define SHADER_TEXT_VS "shaders/text_vs.glsl"
#define SHADER_TEXT_FS "shaders/text_fs.glsl"
#define SHADER_TEXT_ROW_SIZE 4

class TextRender {
	public:
		explicit TextRender(uint32_t width, uint32_t height);
		TextRender(TextRender const &src);
		virtual ~TextRender();

		TextRender &operator=(TextRender const &rhs);
		void loadFont(std::string name, std::string const &filename, uint32_t size);
		void write(std::string const &fontName, std::string text, GLfloat x = 0, GLfloat y = 0, GLfloat scale = 1,
			glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f));
		uint32_t	strWidth(std::string const &fontName, std::string text, GLfloat scale = 1);

		void	setWinSize(glm::vec2 winSize);

		Shader			&getShader();
		Shader const	&getShader() const;

		class TextRenderError : public std::exception {
			public:
				virtual const char* what() const throw() = 0;
		};
		class LoadTextRenderException : public TextRenderError {
			public:
				virtual const char* what() const throw() {
					return ("Fail to load text");
				}
		};
		struct Character {
			GLuint		textureID;
			glm::ivec2	size;
			glm::ivec2	bearing;
			int64_t		advance;
		};
		std::map<std::string, std::map<GLchar, Character> > font;

	private:
		TextRender();

		Shader		_shader;
		glm::mat4	_projection;
		GLuint		_vao;
		GLuint		_vbo;
};
