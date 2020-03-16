#include "ABaseUI.hpp"
#include "Logging.hpp"

ABaseUI::ABaseUI(glm::vec2 pos, glm::vec2 size)
: _enabled(true),
  _pos(pos),
  _posOffset(glm::vec2(0, 0)),
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
  _imgDefSize({0, 0}),
  _isClickableUI(true),
  _mouseHover(false),
  _rightClick(false),
  _keyRightClickBindScancode(NO_SCANCODE),
  _keyRightClickBindInput(InputType::NO_KEY),
  _leftClick(false),
  _keyLeftClickBindScancode(NO_SCANCODE),
  _keyLeftClickBindInput(InputType::NO_KEY),
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
		logWarn("UI object copied");
		_pos = rhs._pos;
		_size = rhs._size;
	}
	return *this;
}

/**
 * @brief This is the base update function of UI objects
 */
void ABaseUI::update() {
	if (!_enabled)
		return;
	if (_isClickableUI) {
		// buttons calculation only if the UI is clickable
		_updateClick();
	}
	// update of UI element
	_update();
}

/**
 * @brief Called by update if UI is clickable. Update mousehover, mouseclick, ...
 */
void ABaseUI::_updateClick() {
	glm::vec2 mousePos = Inputs::getMousePos();
	mousePos.y = _winSize.y - mousePos.y;

	/* state of the keys */
	// right
	bool keyRightDown = false;
	if (_keyRightClickBindScancode != NO_SCANCODE && Inputs::getKeyByScancodeDown(_keyRightClickBindScancode))
		keyRightDown = true;
	if (_keyRightClickBindInput != InputType::NO_KEY && Inputs::getKeyDown(_keyRightClickBindInput))
		keyRightDown = true;
	bool keyRightUp = false;
	if (_keyRightClickBindScancode != NO_SCANCODE && Inputs::getKeyByScancodeUp(_keyRightClickBindScancode))
		keyRightUp = true;
	if (_keyRightClickBindInput != InputType::NO_KEY && Inputs::getKeyUp(_keyRightClickBindInput))
		keyRightUp = true;
	// left
	bool keyLeftDown = false;
	if (_keyLeftClickBindScancode != NO_SCANCODE && Inputs::getKeyByScancodeDown(_keyLeftClickBindScancode))
		keyLeftDown = true;
	if (_keyLeftClickBindInput != InputType::NO_KEY && Inputs::getKeyDown(_keyLeftClickBindInput))
		keyLeftDown = true;
	bool keyLeftUp = false;
	if (_keyLeftClickBindScancode != NO_SCANCODE && Inputs::getKeyByScancodeUp(_keyLeftClickBindScancode))
		keyLeftUp = true;
	if (_keyLeftClickBindInput != InputType::NO_KEY && Inputs::getKeyUp(_keyLeftClickBindInput))
		keyLeftUp = true;

	if (mousePos.x >= getRealPos().x && mousePos.x <= getRealPos().x + _size.x
	&& mousePos.y >= getRealPos().y && mousePos.y <= getRealPos().y + _size.y)
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

	if (keyRightDown) {
		_rightClick = true;
	}
	if (keyLeftDown) {
		_leftClick = true;
	}

	if (Inputs::getLeftClickUp() || keyLeftUp) {
		if ((_mouseHover || keyLeftUp) && _leftClick && _leftListener)
			*_leftListener = _leftClick;
		_leftClick = false;
	}
	if (Inputs::getRightClickUp() || keyRightUp) {
		if ((_mouseHover || keyRightUp) && _rightClick && _rightListener)
			*_rightListener = _rightClick;
		_rightClick = false;
	}
}

/**
 * @brief This is the base draw function of UI objects
 */
void ABaseUI::draw() {
	if (!_enabled)
		return;

	if (_showHelp) {
		/* get shortcut if exist */
		std::string helpText = "";
		if (_keyLeftClickBindInput != InputType::NO_KEY)
			helpText = Inputs::getKeyName(_keyLeftClickBindInput);
		else if (_keyLeftClickBindScancode != NO_SCANCODE)
			helpText = Inputs::getScancodeName(_keyLeftClickBindScancode);

		/* show shortcut */
		if (helpText != "") {
			glm::vec2 tmpPos = getRealPos();
			glm::vec2 tmpSize = _size;

			/* get text informations */
			uint32_t width = _textRender->strWidth(_helpFont, helpText, _helpTextScale);
			uint32_t height = _textRender->strHeight(_helpFont, helpText, _helpTextScale);

			tmpSize = glm::vec2(width + _helpPadding, height + _helpPadding);
			tmpPos.x = getRealPos().x + _size.x - tmpSize.x - _borderSize - _helpPadding;
			tmpPos.y = getRealPos().y + _borderSize + _helpPadding;

			_drawText(tmpPos, tmpSize, _helpFont, _helpTextScale, helpText, _textColor, TextAlign::CENTER, 0);

			_drawBorderRect(tmpPos, tmpSize, _helpBorderSize, _borderColor);
			_drawRect(tmpPos, tmpSize, _color);
		}
	}

	// draw the UI element
	_draw();
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
	if (_rightListener)
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
	if (_leftListener)
		*_leftListener = _leftClick;
	return *this;
}

/* setter */
ABaseUI &	ABaseUI::setKeyRightClickScancode(SDL_Scancode scancode) {
	_keyRightClickBindScancode = scancode; return *this;
}
ABaseUI &	ABaseUI::setKeyLeftClickScancode(SDL_Scancode scancode) {
	_keyLeftClickBindScancode = scancode; return *this;
}
ABaseUI &	ABaseUI::setKeyRightClickInput(InputType::Enum input) {
	_keyRightClickBindInput = input; return *this;
}
ABaseUI &	ABaseUI::setKeyLeftClickInput(InputType::Enum input) {
	_keyLeftClickBindInput = input; return *this;
}

ABaseUI &	ABaseUI::setEnabled(bool enabled) { _enabled = enabled; return *this; }
ABaseUI &	ABaseUI::setPos(glm::vec2 pos) { _pos = pos; return *this; }
ABaseUI &	ABaseUI::setPosOffset(glm::vec2 offset) { _posOffset = offset; return *this; }
ABaseUI &	ABaseUI::addPosOffset(glm::vec2 offset) { _posOffset += offset; return *this; }
ABaseUI &	ABaseUI::setSize(glm::vec2 size) { _size = size; return *this; }
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

bool				ABaseUI::isEnabled() const { return _enabled; }
glm::vec2 &			ABaseUI::getPos() { return _pos; }
glm::vec2 const &	ABaseUI::getPos() const { return _pos; }
glm::vec2			ABaseUI::getRealPos() const { return _pos + _posOffset; }
glm::vec2 &			ABaseUI::getSize() { return _size; }
glm::vec2 const &	ABaseUI::getSize() const { return _size; }
Shader &			ABaseUI::getRectShader() { return *_rectShader; }

/* exception */
ABaseUI::UIException::UIException()
: std::runtime_error("UI Exception") {}

ABaseUI::UIException::UIException(const char* what_arg)
: std::runtime_error(std::string(std::string("UIError: ") + what_arg).c_str()) {}
