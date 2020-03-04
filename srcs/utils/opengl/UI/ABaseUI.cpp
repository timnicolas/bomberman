#include "ABaseUI.hpp"
#include "Logging.hpp"

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
TextRender *	ABaseUI::_textRender = nullptr;

void ABaseUI::init(std::string const & fontName, uint32_t fontSize) {
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

	// disable shader
	_rectShader->unuse();

	/* create text render */
	_textRender = new TextRender(0, 0);
	try {
		_textRender->loadFont("font", fontName, fontSize);
	}
	catch (TextRender::TextRenderError & e) {
		throw UIException(e.what());
	}
}

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
}

ABaseUI::ABaseUI(glm::vec2 winSize, glm::vec2 pos, glm::vec2 size)
: _pos(pos),
  _size(size),
  _color(1.0, 1.0, 1.0, 1.0),
  _borderColor(0.0, 0.0, 0.0, 1.0),
  _borderSize(5.0),
  _text("default text"),
  _textColor(0.0, 0.0, 0.0, 1.0),
  _textScale(1.0)
{
	setWinSize(winSize);
}

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

/*
	call this function on every window resize
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
	_textRender->setWinSize(winSize);
}

/*
	draw a rect at `pos` of size `size` with the color `color`
*/
void ABaseUI::drawRect(glm::vec2 pos, glm::vec2 size, glm::vec4 color) {
	_rectShader->use();

	// set color
    _rectShader->setVec4("color", color);  // set color

	// set model matrix
	glm::mat4 model = glm::mat4(1.0);
	model = glm::translate(model, glm::vec3(pos.x, pos.y, 0));
	model = glm::scale(model, glm::vec3(size.x, size.y, 0));
	_rectShader->setMat4("model", model);

	// draw triangles
	glBindVertexArray(_rectVao);
	glDrawArrays(GL_TRIANGLES, 0, 6);  // draw
	glBindVertexArray(0);

    _rectShader->unuse();
}

/*
	draw text centered on a pos
		pos -> position of the center of the text
		scale -> size of the text
		text -> the text to print
*/
void ABaseUI::drawTextCenter(glm::vec2 pos, float scale, std::string const & text, glm::vec4 color) {
	_textRender->write("font", text, pos.x, pos.y, scale, color);
}

/* setter */
ABaseUI &	ABaseUI::setColor(glm::vec4 color) { _color = color; return *this; }
ABaseUI &	ABaseUI::setBorderColor(glm::vec4 color) { _borderColor = color; return *this; }
ABaseUI &	ABaseUI::setBorderSize(float size) { _borderSize = size; return *this; }

ABaseUI &	ABaseUI::setText(std::string const & text) { _text = text; return *this; }
ABaseUI &	ABaseUI::setTextColor(glm::vec4 color) { _textColor = color; return *this; }
ABaseUI &	ABaseUI::setTextScale(float scale) { _textScale = scale; return *this; }

/* getter */
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
