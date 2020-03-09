#include "ABaseUI.hpp"
#include "Logging.hpp"
#include "debug.hpp"
#include "Texture.hpp"
#include "Inputs.hpp"

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

/**
 * @brief init the UI interface
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
	catch (TextRender::TextRenderError & e) {
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
 *
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
	catch (TextRender::TextRenderError & e) {
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


ABaseUI::ABaseUI(glm::vec2 pos, glm::vec2 size)
: _pos(pos),
  _size(size),
  _color(1.0, 1.0, 1.0, 1.0),
  _borderColor(0.0, 0.0, 0.0, 1.0),
  _borderSize(5.0),
  _mouseHoverColor(0.0, 0.0, 0.0, 0.2),
  _mouseClickColor(0.0, 0.0, 0.0, 0.5),
  _text("default text"),
  _textColor(0.0, 0.0, 0.0, 1.0),
  _textFont(_defFont),
  _textScale(1.0),
  _textPadding(5),
  _textAlign(TextAlign::CENTER),
  _imgTextureID(0),
  _imgDefWidth(0),
  _imgDefHeight(0),
  _mouseHover(false),
  _rightClick(false),
  _leftClick(false),
  _rightListener(nullptr),
  _leftListener(nullptr)
{}

ABaseUI::ABaseUI(ABaseUI const & src) {
	*this = src;
}

ABaseUI::~ABaseUI() {
}

ABaseUI & ABaseUI::operator=(ABaseUI const & rhs) {
	if (this != &rhs) {
		_pos = rhs._pos;
		_size = rhs._size;
	}
	return *this;
}

/**
 * @brief this is the base update function of UI objects
 *
 * @param mousePos the position of the mouse
 * @param rightClick a boolean to know if right click is pressed
 * @param leftClick a boolean to know if left click is pressed
 */
void ABaseUI::update(glm::vec2 mousePos, bool rightClick, bool leftClick) {
	mousePos.y = _winSize.y - mousePos.y;
	if (mousePos.x >= _pos.x && mousePos.x <= _pos.x + _size.x
	&& mousePos.y >= _pos.y && mousePos.y <= _pos.y + _size.y)
	{
		_mouseHover = true;
		if (Inputs::getLeftClickDown()) {
			_leftClick = true;
		}
		if (Inputs::getRightClickDown()) {
			_rightClick = true;
		}
	}
	else {
		_mouseHover = false;
	}
	if (Inputs::getLeftClickUp()) {
		if (_leftClick && _leftListener)
			*_leftListener = _leftClick;
		_leftClick = false;
	}
	if (Inputs::getRightClickUp()) {
		if (_rightClick && _rightListener)
			*_rightListener = _rightClick;
		_rightClick = false;
	}
	_update(mousePos, rightClick, leftClick);
}

/* listener */
/**
 * @brief add a listener on the right click
 *
 * @param listener a pointer on the bool listener
 * @return ABaseUI& a reference to the object
 */
ABaseUI &	ABaseUI::addButtonRightListener(bool * listener) {
	_rightListener = listener;
	*_rightListener = _rightClick;
	return *this;
}
/**
 * @brief add a listener on the left click
 *
 * @param listener a pointer on the bool listener
 * @return ABaseUI& a reference to the object
 */
ABaseUI &	ABaseUI::addButtonLeftListener(bool * listener) {
	_leftListener = listener;
	*_leftListener = _leftClick;
	return *this;
}

/* setter */
ABaseUI &	ABaseUI::setColor(glm::vec4 color) { _color = color; return *this; }

ABaseUI &	ABaseUI::setBorderColor(glm::vec4 color) { _borderColor = color; return *this; }
ABaseUI &	ABaseUI::setBorderSize(float size) { _borderSize = size; return *this; }

ABaseUI &	ABaseUI::setMouseHoverColor(glm::vec4 color) { _mouseHoverColor = color; return *this; }
ABaseUI &	ABaseUI::setMouseClickColor(glm::vec4 color) { _mouseClickColor = color; return *this; }

ABaseUI &	ABaseUI::setText(std::string const & text) { _text = text; return *this; }
ABaseUI &	ABaseUI::setTextColor(glm::vec4 color) { _textColor = color; return *this; }
ABaseUI &	ABaseUI::setTextFont(std::string const & font) { _textFont = font; return *this; }
ABaseUI &	ABaseUI::setTextScale(float scale) { _textScale = scale; return *this; }
ABaseUI &	ABaseUI::setTextPadding(float padding) { _textPadding = padding; return *this; }
ABaseUI &	ABaseUI::setTextAlign(TextAlign::Enum align) { _textAlign = align; return *this; }

/* getter */
bool				ABaseUI::getMouseHover() const { return _mouseHover; }
bool				ABaseUI::getMouseRightClick() const { return _rightClick; }
bool				ABaseUI::getMouseLeftClick() const { return _leftClick; }

glm::vec2 &			ABaseUI::getPos() { return _pos; }
glm::vec2 const &	ABaseUI::getPos() const { return _pos; }
glm::vec2 &			ABaseUI::getSize() { return _size; }
glm::vec2 const &	ABaseUI::getSize() const { return _size; }
Shader &			ABaseUI::getRectShader() { return *_rectShader; }

/* exception */
ABaseUI::UIException::UIException()
: std::runtime_error("UI Exception") {}

ABaseUI::UIException::UIException(const char* what_arg)
: std::runtime_error(std::string(std::string("UIError: ") + what_arg).c_str()) {}
