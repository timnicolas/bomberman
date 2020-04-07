#include "ABaseUI.hpp"
#include "ABaseMasterUI.hpp"
#include "Logging.hpp"

ABaseUI::ABaseUI(glm::vec2 pos, glm::vec2 size)
: _enabled(true),
  _pos(pos),
  _posOffset(glm::vec2(0, 0)),
  _size(size),
  _color(0.0, 0.0, 0.0, 1.0),
  _value(0),
  _borderColor(0.0, 0.0, 0.0, 1.0),
  _borderSize(2.0),
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
  _leftListener(nullptr),
  _leftValueListener(nullptr),
  _master(nullptr)
{
	if (!_isInit) {
		logErr("You need to call ABaseUI::init() before creating UI objects");
		return;
	}
	_allUI.push_back(this);  // add pointer to the new UI in _allUI
}

ABaseUI::ABaseUI(ABaseUI const & src) {
	*this = src;
}

ABaseUI::~ABaseUI() {
	// remove reference in master
	setMaster(nullptr);
	// remove the reference to this UI
	auto it = std::find(_allUI.begin(), _allUI.end(), this);
	if (it == _allUI.end()) {
		logErr("unable to find UI reference in allUI list");
	}
	else {
		_allUI.erase(it);
	}
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
	if (!_enabled || isTotallyOutOfScreen() || isPartiallyOutOfMaster())
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
		if ((_mouseHover || keyLeftUp) && _leftClick && _leftValueListener)
			*_leftValueListener = _value;
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
	if (!_enabled || isTotallyOutOfScreen() || isPartiallyOutOfMaster())
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

/**
 * @brief Called in setWinSize for all UI
 *
 * @param winScale2f Scale for new window size (vec2)
 * @param winScale1f Scale for new window size (float)
 */
void ABaseUI::_resizeWin(glm::vec2 const & winScale2f, float winScale1f) {
	// basics
	_pos *= winScale2f;
	_posOffset *= winScale2f;
	_size *= winScale2f;
	// border
	_borderSize *= winScale1f;
	// text
	_textScale *= winScale1f;
	_textPadding *= winScale1f;
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

/**
 * @brief add a listener on the left click who will be modify to value
 *
 * @param listener a pointer on te int64_t listener
 * @param value the value the the listener will take.
 * @return ABaseUI& a reference to the object
 */
ABaseUI &	ABaseUI::addButtonLeftValueListener(int64_t * listener, int64_t value) {
	_leftValueListener = listener;
	_value = value;
	return *this;
}

// -- setter -------------------------------------------------------------------
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

/**
 * @brief Check if the UI element is partially out of the screen
 *
 * @return true if partially out of the screen
 */
bool	ABaseUI::isPartiallyOutOfScreen() const {
	glm::vec2 rpos = getRealPos();
	if (rpos.x < 0 || rpos.y < 0 || rpos.x + _size.x > _winSize.x || rpos.y + _size.y > _winSize.y)
		return true;
	return false;
}
/**
 * @brief Check if the UI element is totally out of the screen
 *
 * @return true if totally out of the screen
 */
bool	ABaseUI::isTotallyOutOfScreen() const {
	glm::vec2 rpos = getRealPos();
	if (rpos.x + _size.x < 0 || rpos.y + _size.y < 0 || rpos.x > _winSize.x || rpos.y > _winSize.y)
		return true;
	return false;
}
/**
 * @brief Check if the UI element is partially out of the master element
 *
 * @return true if partially out of the master element
 * @return false if totally on master or if master doesn't exist
 */
bool	ABaseUI::isPartiallyOutOfMaster() const {
	if (_master == nullptr)
		return false;
	// relative pos in master
	glm::vec2 posInMaster = getRealPos() - _master->getMasterPos();
	// master size
	glm::vec2 mSize = _master->getMasterSize();
	if (posInMaster.x < 0 || posInMaster.y < 0 || posInMaster.x + _size.x > mSize.x || posInMaster.y + _size.y > mSize.y)
		return true;
	return false;
}
/**
 * @brief Check if the UI element is totally out of the master element
 *
 * @return true if totally out of the master element
 * @return false if partially or totally on master or if master doesn't exist
 */
bool	ABaseUI::isTotallyOutOfMaster() const {
	if (_master == nullptr)
		return false;
	// relative pos in master
	glm::vec2 posInMaster = getRealPos() - _master->getMasterPos();
	// master size
	glm::vec2 mSize = _master->getMasterSize();
	if (posInMaster.x + _size.x < 0 || posInMaster.y + _size.y < 0 || posInMaster.x > mSize.x || posInMaster.y > mSize.y)
		return true;
	return false;
}

ABaseUI &	ABaseUI::setEnabled(bool enabled) { _enabled = enabled; return *this; }
ABaseUI &	ABaseUI::setPos(glm::vec2 pos) { _pos = pos; return *this; }
ABaseUI &	ABaseUI::setPosOffset(glm::vec2 offset) { _posOffset = offset; return *this; }
ABaseUI &	ABaseUI::addPosOffset(glm::vec2 offset) { _posOffset += offset; return *this; }
ABaseUI &	ABaseUI::setSize(glm::vec2 size) { _size = size; return *this; }
ABaseUI &	ABaseUI::setCalculatedSize() {
	uint32_t width = _textRender->strWidth(_textFont, _text, _textScale);
	uint32_t height = _textRender->strHeight(_textFont, _text, _textScale);
	_size = glm::vec2(width + _textPadding * 2, height + _textPadding * 2);;
	return *this;
}
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

ABaseUI &	ABaseUI::setMaster(ABaseMasterUI * master) {
	if (_master != nullptr)
		_master->removeChild(this);
	_master = master;
	if (_master != nullptr)
		_master->addChild(this);
	return *this;
}

/* getter */
bool				ABaseUI::getMouseHover() const { return _mouseHover; }
bool				ABaseUI::getMouseRightClick() const { return _rightClick; }
bool				ABaseUI::getMouseLeftClick() const { return _leftClick; }

bool				ABaseUI::isEnabled() const { return _enabled; }
glm::vec2 &			ABaseUI::getPos() { return _pos; }
glm::vec2 const &	ABaseUI::getPos() const { return _pos; }
glm::vec2			ABaseUI::getRealPos() const {
	glm::vec2 masterPos = (_master) ? _master->getMasterRealPos() : glm::vec2(0, 0);
	return masterPos + _pos + _posOffset;
}
glm::vec2 &			ABaseUI::getSize() { return _size; }
glm::vec2 const &	ABaseUI::getSize() const { return _size; }
Shader &			ABaseUI::getRectShader() { return *_rectShader; }
uint32_t			ABaseUI::getTextWidth() const {
	return _textRender->strWidth(_textFont, _text, _textScale) + _textPadding * 2;
}

// -- exception ----------------------------------------------------------------
ABaseUI::UIException::UIException()
: std::runtime_error("UI Exception") {}

ABaseUI::UIException::UIException(const char* what_arg)
: std::runtime_error(std::string(std::string("UIError: ") + what_arg).c_str()) {}
