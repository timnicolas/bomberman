#include "ABaseUI.hpp"
#include "Logging.hpp"
#include "debug.hpp"
#include "Texture.hpp"

/**
 * @brief This file contains all static variables and methods for ABaseUI
 */

/* global */
glm::vec2		ABaseUI::_winSize;
glm::mat4		ABaseUI::_projection;
/* rect */
Shader *		ABaseUI::_rectShader = nullptr;
GLuint			ABaseUI::_rectVao = 0;
GLuint			ABaseUI::_rectVbo = 0;
const float		ABaseUI::_rectVertices[] = {
	0.0, 0.0,
	1.0, 1.0,
	0.0, 1.0,

	0.0, 0.0,
	1.0, 0.0,
	1.0, 1.0,
};
/* text */
TextRender *	ABaseUI::_textRender = nullptr;
std::string		ABaseUI::_defFont = "default";
/* img */
Shader *		ABaseUI::_imgShader = nullptr;
GLuint			ABaseUI::_imgVao = 0;
GLuint			ABaseUI::_imgVbo = 0;
const float		ABaseUI::_imgVertices[] = {
//  pos      | texture coord
	0.0, 1.0,  0.0, 0.0,
	0.0, 0.0,  0.0, 1.0,
	1.0, 0.0,  1.0, 1.0,

	0.0, 1.0,  0.0, 0.0,
	1.0, 0.0,  1.0, 1.0,
	1.0, 1.0,  1.0, 0.0,
};
/* help */
bool			ABaseUI::_showHelp = false;
std::string		ABaseUI::_helpFont = ABaseUI::_defFont;
float			ABaseUI::_helpTextScale = 0.8;
float			ABaseUI::_helpBorderSize = 1;
float			ABaseUI::_helpPadding = 8;
SDL_Scancode	ABaseUI::_helpKeyBindScancode = NO_SCANCODE;
InputType::Enum	ABaseUI::_helpKeyBindInput = InputType::NO_KEY;

/**
 * @brief init the UI interface.
 *
 * Call this function at program startup.
 *
 * @param winSize the size of the window
 * @param defFontName the defeult font filename
 * @param defFontSize the default font size
 */
void ABaseUI::init(glm::vec2 winSize, std::string const & defFontName, uint32_t defFontSize) {
	if (_rectShader != nullptr) {
		logWarn("call ABaseUI::init only once");
		return;
	}

	/* create rect shader */
	// create shader
	_rectShader = new Shader(SHADER_RECT_2D_VS, SHADER_RECT_2D_FS);
	// enable shader
	_rectShader->use();

	// create VAO & VBO
	glGenVertexArrays(1, &_rectVao);
	glGenBuffers(1, &_rectVbo);

	// enable vao & vbo
	glBindVertexArray(_rectVao);
	glBindBuffer(GL_ARRAY_BUFFER, _rectVbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(_rectVertices), _rectVertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, SHADER_RECT_2D_ROW_SZ * sizeof(float), reinterpret_cast<void*>(0));
	glEnableVertexAttribArray(0);

	// disable vao & vbo
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	// send default values to uniforms
	_rectShader->setMat4("model", glm::mat4(1.0));
    _rectShader->setVec4("masterColor", glm::vec4(1.0, 1.0, 1.0, 1.0));
    _rectShader->setVec4("secondColor", glm::vec4(1.0, 1.0, 1.0, 1.0));
    _rectShader->setFloat("colorFactor", 1);

	// disable shader
	_rectShader->unuse();

	/* create text render */
	_textRender = new TextRender(0, 0);
	try {
		_textRender->loadFont(_defFont, defFontName, defFontSize);
	}
	catch (TextRender::TextRenderError const & e) {
		throw UIException(e.what());
	}

	/* create image shader */
	// create shader
	_imgShader = new Shader(SHADER_IMAGE_2D_VS, SHADER_IMAGE_2D_FS);
	// enable shader
	_imgShader->use();

	// create VAO & VBO
	glGenVertexArrays(1, &_imgVao);
	glGenBuffers(1, &_imgVbo);

	// enable veo & vbo
	glBindVertexArray(_imgVao);
	glBindBuffer(GL_ARRAY_BUFFER, _imgVbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * SHADER_IMAGE_2D_ROW_SIZE, _imgVertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, SHADER_IMAGE_2D_ROW_SIZE * sizeof(float), reinterpret_cast<void*>(0));
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, SHADER_IMAGE_2D_ROW_SIZE * sizeof(float),
							reinterpret_cast<void*>(2 * sizeof(float)));
	glEnableVertexAttribArray(1);

	// disable vao & vbo
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	// send default values to uniforms
	_imgShader->setMat4("model", glm::mat4(1.0));
    _imgShader->setVec4("color", glm::vec4(0.0, 0.0, 0.0, 0.0));

	// disable shader
	_imgShader->unuse();

	// set projection matrix
	setWinSize(winSize);
}

/**
 * @brief call this function at the end of the program to free UI interfaces
 */
void ABaseUI::destroy() {
	if (_rectShader == nullptr) {
		logWarn("call ABaseUI::destroy only once and only if you have already called ABaseUI::init");
		return;
	}
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDeleteVertexArrays(1, &_rectVao);
	glDeleteBuffers(1, &_rectVbo);
	delete _rectShader;
	_rectShader = nullptr;
	glDeleteVertexArrays(1, &_imgVao);
	glDeleteBuffers(1, &_imgVbo);
	delete _imgShader;
	_imgShader = nullptr;
}

/**
 * @brief Call this function foreach loop when updating
 */
void ABaseUI::staticUpdate() {
	/* help */
	if (_helpKeyBindScancode != NO_SCANCODE && Inputs::getKeyByScancodeUp(_helpKeyBindScancode)) {
		_showHelp = !_showHelp;
	}
	if (_helpKeyBindInput != InputType::NO_KEY && Inputs::getKeyUp(_helpKeyBindInput)) {
		_showHelp = !_showHelp;
	}
}

/**
 * @brief load a new font
 *
 * @param fontName the font name
 * @param filename the ttf file to load
 * @param fontSize the size of the font
 *
 * @throw UIException if the font failed to load
 */
void ABaseUI::loadFont(std::string const & fontName, std::string const & filename, uint32_t fontSize) {
	try {
		_textRender->loadFont(fontName, filename, fontSize);
	}
	catch (TextRender::TextRenderError const & e) {
		throw UIException(e.what());
	}
}

/**
 * @brief call this function on every window resize to update projection matrix
 *
 * @param winSize the size of the window
 */
void ABaseUI::setWinSize(glm::vec2 winSize) {
	_winSize = winSize;
	_projection = glm::ortho(
		0.0f,
		static_cast<GLfloat>(_winSize.x),
		0.0f,
		static_cast<GLfloat>(_winSize.y));
	_rectShader->use();
	_rectShader->setMat4("projection", _projection);
	_rectShader->unuse();
	_imgShader->use();
	_imgShader->setMat4("projection", _projection);
	_imgShader->unuse();
	_textRender->setWinSize(winSize);
}

/**
 * @brief Show or hide help
 *
 * @param show Show if true
 */
void ABaseUI::showHelp(bool show) { _showHelp = show; }
/**
 * @brief Change font used for help
 *
 * @param fontName The name of the font (set name in ABaseUI::loadFont)
 */
void ABaseUI::setHelpFont(std::string fontName) { _helpFont = fontName; }
/**
 * @brief Set a scancode to toogle help on UI
 *
 * @param scancode The scancode
 */
void ABaseUI::setHelpToogleScancode(SDL_Scancode scancode) { _helpKeyBindScancode = scancode; }
/**
 * @brief Set an InputType to toogle help on UI
 *
 * @param input The InputType
 */
void ABaseUI::setHelpToogleInput(InputType::Enum input) { _helpKeyBindInput = input; }
/**
 * @brief Set the text size for help
 *
 * @param scale The new scale (1 to set default text scale)
 */
void ABaseUI::setHelpTextScale(float scale) { _helpTextScale = scale; }
/**
 * @brief Set the boder size for the help rectangle
 *
 * @param borderSize The border size
 */
void ABaseUI::setHelpBorderSize(float borderSize) { _helpBorderSize = borderSize; }
