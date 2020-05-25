#pragma once

#include <freetype2/ft2build.h>
#include FT_FREETYPE_H
#include <stdexcept>
#include <map>
#include "includesOpengl.hpp"
#include "Shader.hpp"

#define SHADER_TEXT_VS "shaders/text_vs.glsl"
#define SHADER_TEXT_FS "shaders/text_fs.glsl"
#define SHADER_TEXT_ROW_SIZE 5

/**
 * @brief render 2D text on an openGL 3D context
 */
class TextRender {
	public:
		TextRender(uint32_t width, uint32_t height);
		TextRender(TextRender const &src);
		virtual ~TextRender();

		TextRender &operator=(TextRender const &rhs);
		void loadFont(std::string name, std::string const &filename, uint32_t size);
		void write(std::string const &fontName, std::string text, glm::vec3 pos, GLfloat scale = 1,
			glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f), int outline = 0,
			glm::vec3 colorOutline = glm::vec3(1.0f, 1.0f, 1.0f));
		uint32_t	strWidth(std::string const &fontName, std::string text, GLfloat scale = 1);
		uint32_t	strHeight(std::string const &fontName, GLfloat scale = 1, bool fullHeight = false);

		void	setWinSize(glm::vec2 winSize);

		Shader			&getShader();
		Shader const	&getShader() const;

		/**
		 * @brief Text render exception
		 */
		class TextRenderError : public std::exception {
			public:
				/**
				 * @brief Function auto called on errors
				 *
				 * @return const char* Error message
				 */
				virtual const char* what() const throw() = 0;
		};
		/**
		 * @brief Exception when loading text (TextRender)
		 */
		class LoadTextRenderException : public TextRenderError {
			public:
				/**
				 * @brief Function auto called on errors
				 *
				 * @return const char* Error message
				 */
				virtual const char* what() const throw() {
					return ("Fail to load text");
				}
		};
		/**
		 * @brief All information about char (foreach characters)
		 */
		struct Character {
			GLuint		textureID;  /**< Texture ID */
			glm::ivec2	size;  /**< Char size */
			glm::ivec2	bearing;  /**< Char bearing */
			int64_t		advance;  /**< Char advance */
		};
		std::map<std::string, std::map<GLchar, Character> > font;  /**< All character info foreach fonts */

	private:
		TextRender();

		void		_writeChar(Character c, glm::vec3 pos, GLfloat scale);

		Shader		_shader;  /**< TextRender shader */
		glm::mat4	_projection;  /**< Projection matrix */
		GLuint		_vao;  /**< Vertex Array Objects */
		GLuint		_vbo;  /**< Vertex Buffer Objects */
};
