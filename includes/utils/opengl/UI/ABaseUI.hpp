#pragma once

#include "includesOpengl.hpp"
#include "Shader.hpp"
#include "TextRender.hpp"
#include "Inputs.hpp"

/* def values */
#define UI_DEF_Z					0
#define UI_DEF_POS_OFFSET			glm::vec2(0, 0)
#define UI_DEF_COLOR				glm::vec4(0.0, 0.0, 0.0, 1.0)
#define UI_DEF_VALUE				0
#define UI_DEF_BORDER_COLOR			glm::vec4(0.0, 0.0, 0.0, 1.0)
#define UI_DEF_BORDER_SIZE			2.0
#define UI_DEF_MOUSE_HOVER_COLOR	glm::vec4(0.0, 0.0, 0.0, 0.2)
#define UI_DEF_MOUSE_CLICK_COLOR	glm::vec4(0.0, 0.0, 0.0, 0.5)
#define UI_DEF_TEXT					""
#define UI_DEF_TEXT_COLOR			glm::vec4(0.0, 0.0, 0.0, 1.0)
#define UI_DEF_TEXT_FOND			"default"
#define UI_DEF_TEXT_SCALE			1.0
#define UI_DEF_TEXT_PADDING			5
#define UI_DEF_TEXT_ALIGN			TextAlign::CENTER

/* rect */
#define SHADER_RECT_2D_VS "./shaders/rect_2D_vs.glsl"
#define SHADER_RECT_2D_FS "./shaders/rect_2D_fs.glsl"
#define SHADER_RECT_2D_ROW_SZ 2

/* img */
#define SHADER_IMAGE_2D_VS "./shaders/image_2D_vs.glsl"
#define SHADER_IMAGE_2D_FS "./shaders/image_2D_fs.glsl"
#define SHADER_IMAGE_2D_ROW_SIZE 4
#define VOID_SIZE glm::vec2 {-1, -1}

/**
 * @brief this is the text alignment (LEFT RIGHT or CENTER)
 */
namespace TextAlign {
	enum Enum {
		LEFT,
		CENTER,
		RIGHT,
	};
}

class ABaseMasterUI;

/**
 * @brief this is the base UI interface
 *
 * You can re-implement this class to create your UI elements.
 * This base class implement functions to draw rectangles, images, text, ...
 * You need to call some static functions to have functional UI
 *  - ABaseUI::init(...) once on startup
 *  - ABaseUI::staticUpdate() in each loop
 *  - ABaseUI::destroy() at the end
 */
class ABaseUI {
	public:
		/* static functions */
		static void init(glm::vec2 winSize, std::string const & fontName, uint32_t fontSize);
		static void destroy();
		static void staticUpdate();
		static void	setWinSize(glm::vec2 winSize);
		static void loadFont(std::string const & fontName, std::string const & filename, uint32_t fontSize);
		static uint32_t strWidth(std::string const & fontName, std::string const & txt, float scale = 1);
		static uint32_t strWidth(std::string const & txt, float scale = 1);
		static uint32_t strHeight(std::string const & fontName, float scale = 1, bool fullHeight = false);
		static uint32_t strHeight(float scale = 1, bool fullHeight = false);
		/* help */
		static void showHelp(bool show);
		static void setHelpFont(std::string fontName);
		static void setHelpToogleScancode(SDL_Scancode scancode);
		static void setHelpToogleInput(InputType::Enum input);
		static void setHelpTextScale(float scale);
		static void setHelpBorderSize(float borderSize);

		/* base functions */
		ABaseUI(glm::vec2 pos, glm::vec2 size);
		ABaseUI(ABaseUI const & src);
		virtual ~ABaseUI();

		ABaseUI & operator=(ABaseUI const & rhs);

		/* others */
		void			update();
		void			draw();

		/* listener */
		virtual ABaseUI &			addButtonRightListener(bool * listener);
		virtual ABaseUI &			addButtonLeftListener(bool * listener);
		virtual ABaseUI &			addButtonLeftValueListener(int64_t * listener, int64_t value);

		/* setter */
		virtual ABaseUI &			setKeyRightClickScancode(SDL_Scancode scancode);
		virtual ABaseUI &			setKeyLeftClickScancode(SDL_Scancode scancode);
		virtual ABaseUI &			setKeyRightClickInput(InputType::Enum input);
		virtual ABaseUI &			setKeyLeftClickInput(InputType::Enum input);

		virtual ABaseUI &			setEnabled(bool enabled);
		virtual ABaseUI &			setPos(glm::vec2 pos);
		virtual ABaseUI &			setZ(float z);
		virtual ABaseUI &			setPosOffset(glm::vec2 offset);
		virtual ABaseUI &			addPosOffset(glm::vec2 offset);
		virtual ABaseUI &			setSize(glm::vec2 size);
		virtual ABaseUI &			setCalculatedSize();
		virtual ABaseUI &			setColor(glm::vec4 color);

		virtual ABaseUI &			setBorderColor(glm::vec4 color);
		virtual ABaseUI &			setBorderSize(float size);

		virtual ABaseUI &			setMouseHoverColor(glm::vec4 color);
		virtual ABaseUI &			setMouseClickColor(glm::vec4 color);

		virtual ABaseUI &			setText(std::string const & text);
		virtual ABaseUI &			setTextColor(glm::vec4 color);
		virtual ABaseUI &			setTextFont(std::string const & font);
		virtual ABaseUI &			setTextScale(float scale);
		virtual ABaseUI &			setTextPadding(float padding);
		virtual ABaseUI &			setTextAlign(TextAlign::Enum align);

		/* master */
		virtual ABaseUI &			setMaster(ABaseMasterUI * master);

		/* check */
		virtual bool				isPartiallyOutOfScreen() const;  // ret true if partially out of the screen
		virtual bool				isTotallyOutOfScreen() const;  // ret true if totally out of the screen
		virtual bool				isPartiallyOutOfMaster() const;  // ret true if partially out of the master element
		virtual bool				isTotallyOutOfMaster() const;  // ret true if totally out of the screen element

		/* getter */
		virtual bool				getMouseHover() const;
		virtual bool				getMouseRightClick() const;
		virtual bool				getMouseLeftClick() const;
		virtual bool				isEnabled() const;
		virtual glm::vec2 &			getPos();
		virtual glm::vec2 const &	getPos() const;
		virtual glm::vec2			getRealPos() const;  // get master + pos + offset
		virtual glm::vec2 &			getSize();
		virtual glm::vec2 const &	getSize() const;
		virtual	uint32_t			getTextWidth() const;
		virtual std::string			getText() const;
		virtual glm::ivec2 &		getImgDefSize();
		virtual glm::ivec2 const &	getImgDefSize() const;
		static Shader &				getRectShader();

		/* Exceptions */
	class UIException : public std::runtime_error {
		public:
			UIException();
			explicit UIException(const char* what_arg);
		};

	protected:
		ABaseUI();
		/* draw base function */
		// rect
		void			_drawRect(glm::vec2 pos, glm::vec2 size, float z, glm::vec4 color1,
			glm::vec4 color2 = glm::vec4(1.0, 1.0, 1.0, 1.0), float factor = 1);
		void			_drawBorderRect(glm::vec2 pos, glm::vec2 size, float z, float borderSize, glm::vec4 color);
		// text
		void			_drawText(glm::vec2 pos, glm::vec2 size, float z, std::string const & font, float scale,
			std::string const & text, glm::vec4 color, TextAlign::Enum align, float padding);
		// img
		void			_loadImg(std::string const & filename, bool updateSize = true);
		void			_unloadImg();
		void			_drawImg(glm::vec2 pos, glm::vec2 size, float z, GLuint textureID, glm::vec4 color);

		// update function (redefined in child class)
		virtual void	_update() = 0;
		virtual void	_draw() = 0;

		// called by setWinSize
		virtual void	_resizeWin(glm::vec2 const & winScale2f, float winScale1f);

		// enable functionalities
		bool			_enabled;
		// basics
		glm::vec2		_pos;
		float			_z;
		glm::vec2		_posOffset;
		glm::vec2		_size;
		glm::vec4		_color;
		int64_t			_value;
		// border
		glm::vec4		_borderColor;
		float			_borderSize;
		// mouse effect
		glm::vec4		_mouseHoverColor;
		glm::vec4		_mouseClickColor;
		// text
		std::string		_text;
		glm::vec4		_textColor;
		std::string 	_textFont;
		float			_textScale;
		float			_textPadding;
		TextAlign::Enum	_textAlign;
		// image
		GLuint			_imgTextureID;
		glm::ivec2		_imgDefSize;

		/* info about mouse */
		bool			_isClickableUI;  // set to false for no buttons UI
		bool			_mouseHover;
		bool			_rightClick;
		SDL_Scancode	_keyRightClickBindScancode;
		InputType::Enum	_keyRightClickBindInput;
		bool			_leftClick;
		SDL_Scancode	_keyLeftClickBindScancode;
		InputType::Enum	_keyLeftClickBindInput;

		/* listener */
		bool *			_rightListener;
		bool *			_leftListener;
		int64_t *		_leftValueListener;

		/* master */
		// master element are an element that contains others
		ABaseMasterUI *	_master;

		/* global */
		static bool						_isInit;
		static std::vector<ABaseUI *>	_allUI;
		/* shaders */
		static glm::vec2				_winSize;
		static glm::mat4				_projection;  // projection matrix (orthogonal)
		/* rectangle */
		static Shader *					_rectShader;
		static GLuint					_rectVao;
		static GLuint					_rectVbo;
		static const float				_rectVertices[];
		/* text */
		static TextRender *				_textRender;
		/* image 2D */
		static Shader *					_imgShader;
		static GLuint					_imgVao;
		static GLuint					_imgVbo;
		static const float				_imgVertices[];
		/* help */
		static bool						_showHelp;
		static std::string				_helpFont;
		static float					_helpTextScale;
		static float					_helpBorderSize;
		static float					_helpPadding;
		static SDL_Scancode				_helpKeyBindScancode;
		static InputType::Enum			_helpKeyBindInput;

	private:
		void			_updateClick();
};
