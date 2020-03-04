#pragma once

#include "includesOpengl.hpp"
#include "Shader.hpp"
#include "TextRender.hpp"

#define SHADER_RECT_2D_VS "./shaders/rect_2D_vs.glsl"
#define SHADER_RECT_2D_FS "./shaders/rect_2D_fs.glsl"
#define SHADER_RECT_2D_ROW_SZ 2

namespace MouseState {
	enum ENUM {
		START_RIGHT_CLICK,
		START_LEFT_CLICK,
		END_RIGHT_CLICK,
		END_LEFT_CLICK,
		NO_CLICK,
	};
};

class ABaseUI {
	public:
		static void init(std::string const & fontName, uint32_t fontSize);
		static void destroy();

		ABaseUI(glm::vec2 winSize, glm::vec2 pos, glm::vec2 size);
		ABaseUI(ABaseUI const & src);
		virtual ~ABaseUI();

		ABaseUI & operator=(ABaseUI const & rhs);

		virtual void		update(glm::vec2 mousePos, MouseState::ENUM mouseState) = 0;
		virtual void		draw() = 0;

		void	setWinSize(glm::vec2 winSize);

		/* draw base function */
		void		drawRect(glm::vec2 pos, glm::vec2 size, glm::vec4 color);
		void		drawTextCenter(glm::vec2 pos, float size, std::string const & text, glm::vec4 color);

		/* setter */
		ABaseUI &	setColor(glm::vec4 color);
		ABaseUI &	setBorderColor(glm::vec4 color);
		ABaseUI &	setBorderSize(float size);

		ABaseUI &	setText(std::string const & text);
		ABaseUI &	setTextColor(glm::vec4 color);
		ABaseUI &	setTextScale(float scale);

		/* getter */
		glm::vec2 &				getPos();
		glm::vec2 const &		getPos() const;
		glm::vec2 &				getSize();
		glm::vec2 const &		getSize() const;
		static Shader &			getRectShader();

		/* Exceptions */
	class UIException : public std::runtime_error {
		public:
			UIException();
			explicit UIException(const char* what_arg);
		};

	protected:
		ABaseUI();

		glm::vec2	_winSize;
		glm::vec2	_pos;
		glm::vec2	_size;
		glm::vec4	_color;
		// border
		glm::vec4	_borderColor;
		float		_borderSize;
		// text
		std::string	_text;
		glm::vec4	_textColor;
		float		_textScale;

		/* shaders */
		glm::mat4			_projection;  // projection matrix (orthogonal)
		static Shader *		_rectShader;
		static GLuint		_rectVao;
		static GLuint		_rectVbo;
		static const float	_rectVertices[];
		static TextRender *	_textRender;
};
