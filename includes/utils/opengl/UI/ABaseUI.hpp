#pragma once

#include "includesOpengl.hpp"
#include "Shader.hpp"
#include "TextRender.hpp"

#define SHADER_RECT_2D_VS "./shaders/rect_2D_vs.glsl"
#define SHADER_RECT_2D_FS "./shaders/rect_2D_fs.glsl"
#define SHADER_RECT_2D_ROW_SZ 2

namespace TextAlign {
	enum Enum {
		LEFT,
		CENTER,
		RIGHT,
	};
}

class ABaseUI {
	public:
		static void init(std::string const & fontName, uint32_t fontSize);
		static void destroy();

		ABaseUI(glm::vec2 winSize, glm::vec2 pos, glm::vec2 size);
		ABaseUI(ABaseUI const & src);
		virtual ~ABaseUI();

		ABaseUI & operator=(ABaseUI const & rhs);

		void			update(glm::vec2 mousePos, bool rightClick, bool leftClick);
		virtual void	draw() = 0;

		void	setWinSize(glm::vec2 winSize);

		/* listener */
		ABaseUI &	addButtonRightListener(bool * listener);
		ABaseUI &	addButtonLeftListener(bool * listener);


		/* setter */
		ABaseUI &	setColor(glm::vec4 color);

		ABaseUI &	setBorderColor(glm::vec4 color);
		ABaseUI &	setBorderSize(float size);

		ABaseUI &	setMouseHoverColor(glm::vec4 color);
		ABaseUI &	setMouseClickColor(glm::vec4 color);

		ABaseUI &	setText(std::string const & text);
		ABaseUI &	setTextColor(glm::vec4 color);
		ABaseUI &	setTextScale(float scale);
		ABaseUI &	setTextPadding(float padding);
		ABaseUI &	setTextAlign(TextAlign::Enum align);

		/* getter */
		bool					getMouseHover() const;
		bool					getMouseRightClick() const;
		bool					getMouseLeftClick() const;
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
		/* draw base function */
		void			_drawRect(glm::vec2 pos, glm::vec2 size, glm::vec4 color1,
			glm::vec4 color2 = glm::vec4(1.0, 1.0, 1.0, 1.0), float factor = 1);
		void			_drawText(glm::vec2 pos, glm::vec2 size, float scale, std::string const & text,
			glm::vec4 color, TextAlign::Enum align, float padding);
		virtual void	_update(glm::vec2 mousePos, bool rightClick, bool leftClick) = 0;

		glm::vec2	_winSize;
		glm::vec2	_pos;
		glm::vec2	_size;
		glm::vec4	_color;
		// border
		glm::vec4	_borderColor;
		float		_borderSize;
		// mouse effect
		glm::vec4	_mouseHoverColor;
		glm::vec4	_mouseClickColor;
		// text
		std::string	_text;
		glm::vec4	_textColor;
		float		_textScale;
		float		_textPadding;
		TextAlign::Enum	_textAlign;

		/* info about mouse */
		bool		_mouseHover;
		bool		_rightClick;
		bool		_leftClick;

		/* listener */
		bool *		_rightListener;
		bool *		_leftListener;

		/* shaders */
		glm::mat4			_projection;  // projection matrix (orthogonal)
		static Shader *		_rectShader;
		static GLuint		_rectVao;
		static GLuint		_rectVbo;
		static const float	_rectVertices[];
		static TextRender *	_textRender;
};