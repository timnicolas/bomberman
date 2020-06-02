#include "ABaseUI.hpp"
#include "ABaseMasterUI.hpp"
#include "Logging.hpp"

/**
 * @brief Construct a new ABaseUI::ABaseUI object
 *
 * @param pos The position of the UI element
 * @param size The size of the UI element
 */
ABaseUI::ABaseUI(glm::vec2 pos, glm::vec2 size)
: _enabled(true),
  _selected(false),
  _pos(pos),
  _z(UI_DEF_Z),
  _posOffset(UI_DEF_POS_OFFSET),
  _size(size),
  _color(UI_DEF_COLOR),
  _value(UI_DEF_VALUE),
  _selectedColor(UI_DEF_SELECTED_COLOR),
  _selectedColorText(UI_DEF_SELECTED_COLOR_TEXT),
  _borderColor(UI_DEF_BORDER_COLOR),
  _borderSize(UI_DEF_BORDER_SIZE),
  _mouseHoverColor(UI_DEF_MOUSE_HOVER_COLOR),
  _mouseHoverColorText(UI_DEF_MOUSE_HOVER_COLOR_TEXT),
  _mouseClickColor(UI_DEF_MOUSE_CLICK_COLOR),
  _mouseClickColorText(UI_DEF_MOUSE_CLICK_COLOR_TEXT),
  _text(UI_DEF_TEXT),
  _textColor(UI_DEF_TEXT_COLOR),
  _textFont(UI_DEF_TEXT_FOND),
  _textScale(UI_DEF_TEXT_SCALE),
  _textPadding(UI_DEF_TEXT_PADDING),
  _textAlign(UI_DEF_TEXT_ALIGN),
  _textOutline(UI_DEF_TEXT_OUTLINE),
  _textOutlineColor(UI_DEF_TEXT_OUTLINE_COLOR),
  _imgTextureID(0),
  _imgHoverTextureID(0),
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

/**
 * @brief Construct a new ABaseUI::ABaseUI object
 *
 * @param src The object to do the copy
 */
ABaseUI::ABaseUI(ABaseUI const & src) {
	*this = src;
}

/**
 * @brief Destroy the ABaseUI::ABaseUI object
 */
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

/**
 * @brief Copy this object
 *
 * @param rhs The object to copy
 * @return ABaseUI& A reference to the copied object
 */
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
			uint32_t height = _textRender->strHeight(_helpFont, _helpTextScale);

			tmpSize = glm::vec2(width + _helpPadding, height + _helpPadding);
			tmpPos.x = getRealPos().x + _size.x - tmpSize.x - _borderSize - _helpPadding;
			tmpPos.y = getRealPos().y + _borderSize + _helpPadding;

			_drawText(tmpPos, tmpSize, _z, _helpFont, _helpTextScale, helpText, _textColor, TextAlign::CENTER, 0);

			_drawBorderRect(tmpPos, tmpSize, _z, _helpBorderSize, _helpBorderColor);
			_drawRect(tmpPos, tmpSize, _z, _color);
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
/**
 * @brief Set the right click shortcut
 *
 * @param scancode The shortcut scancode
 * @return ABaseUI& A reference to this UI element
 */
ABaseUI &	ABaseUI::setKeyRightClickScancode(SDL_Scancode scancode) {
	_keyRightClickBindScancode = scancode; return *this;
}
/**
 * @brief Set the left click shortcut
 *
 * @param scancode The shortcut scancode
 * @return ABaseUI& A reference to this UI element
 */
ABaseUI &	ABaseUI::setKeyLeftClickScancode(SDL_Scancode scancode) {
	_keyLeftClickBindScancode = scancode; return *this;
}
/**
 * @brief Set the right click shortcut
 *
 * @param input The shortcut InputType
 * @return ABaseUI& A reference to this UI element
 */
ABaseUI &	ABaseUI::setKeyRightClickInput(InputType::Enum input) {
	_keyRightClickBindInput = input; return *this;
}
/**
 * @brief Set the left click shortcut
 *
 * @param input The shortcut InputType
 * @return ABaseUI& A reference to this UI element
 */
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

/**
 * @brief Enable / disable UI
 *
 * @param enabled enable
 * @return ABaseUI& A reference to this UI element
 */
ABaseUI &	ABaseUI::setEnabled(bool enabled) { _enabled = enabled; return *this; }
/**
 * @brief Selected / unselected UI
 *
 * @param selected selected
 * @return ABaseUI& A reference to this UI element
 */
ABaseUI &	ABaseUI::setSelected(bool selected) { _selected = selected; return *this; }
/**
 * @brief Set position
 *
 * @param pos Position
 * @return ABaseUI& A reference to this UI element
 */
ABaseUI &	ABaseUI::setPos(glm::vec2 pos) { _pos = pos; return *this; }
/**
 * @brief Set Z (used for transparency)
 *
 * @param z z
 * @return ABaseUI& A reference to this UI element
 */
ABaseUI &	ABaseUI::setZ(float z) { _z = z; return *this; }
/**
 * @brief Set position offset (to move object whitout change position)
 *
 * @param offset The offset
 * @return ABaseUI& A reference to this UI element
 */
ABaseUI &	ABaseUI::setPosOffset(glm::vec2 offset) { _posOffset = offset; return *this; }
/**
 * @brief Add to position offset (to move object whitout change position)
 *
 * @param offset The offset
 * @return ABaseUI& A reference to this UI element
 */
ABaseUI &	ABaseUI::addPosOffset(glm::vec2 offset) { _posOffset += offset; return *this; }
/**
 * @brief Set the object size
 *
 * @param size The new size
 * @return ABaseUI& A reference to this UI element
 */
ABaseUI &	ABaseUI::setSize(glm::vec2 size) { _size = size; return *this; }
/**
 * @brief Auto calculate size from the text size
 *
 * @return ABaseUI& A reference to this UI element
 */
ABaseUI &	ABaseUI::setCalculatedSize() {
	uint32_t width = _textRender->strWidth(_textFont, _text, _textScale);
	uint32_t height = _textRender->strHeight(_textFont, _textScale);
	_size = glm::vec2(width + _textPadding * 2, height + _textPadding * 2);;
	return *this;
}
/**
 * @brief Set the color
 *
 * @param color Color
 * @return ABaseUI& A reference to this UI element
 */
ABaseUI &	ABaseUI::setColor(glm::vec4 color) { _color = color; return *this; }

/**
 * @brief Set the color selected
 *
 * @param color Color selected
 * @return ABaseUI& A reference to this UI element
 */
ABaseUI &	ABaseUI::setSelectedColor(glm::vec4 color) { _selectedColor = color; return *this; }
/**
 * @brief Set the color selected text
 *
 * @param color Color selected text
 * @return ABaseUI& A reference to this UI element
 */
ABaseUI &	ABaseUI::setSelectedColorText(glm::vec4 color) { _selectedColorText = color; return *this; }

/**
 * @brief Set the border color
 *
 * @param color The border color
 * @return ABaseUI& A reference to this UI element
 */
ABaseUI &	ABaseUI::setBorderColor(glm::vec4 color) { _borderColor = color; return *this; }
/**
 * @brief Set the border size
 *
 * @param size The border size
 * @return ABaseUI& A reference to this UI element
 */
ABaseUI &	ABaseUI::setBorderSize(float size) { _borderSize = size; return *this; }

/**
 * @brief Set the mouse hover color
 *
 * @param color Mouse hover color
 * @return ABaseUI& A reference to this UI element
 */
ABaseUI &	ABaseUI::setMouseHoverColor(glm::vec4 color) { _mouseHoverColor = color; return *this; }

/**
 * @brief Set the mouse hover color text
 *
 * @param color Mouse hover color text
 * @return ABaseUI& A reference to this UI element
 */
ABaseUI &	ABaseUI::setMouseHoverColorText(glm::vec4 color) { _mouseHoverColorText = color; return *this; }

/**
 * @brief Set the mouse click color
 *
 * @param color mouse click color
 * @return ABaseUI& A reference to this UI element
 */
ABaseUI &	ABaseUI::setMouseClickColor(glm::vec4 color) { _mouseClickColor = color; return *this; }

/**
 * @brief Set the mouse click color text
 *
 * @param color mouse click color text
 * @return ABaseUI& A reference to this UI element
 */
ABaseUI &	ABaseUI::setMouseClickColorText(glm::vec4 color) { _mouseClickColorText = color; return *this; }

/**
 * @brief Set the text
 *
 * @param text The text
 * @return ABaseUI& A reference to this UI element
 */
ABaseUI &	ABaseUI::setText(std::string const & text) { _text = text; return *this; }
/**
 * @brief Set the text color
 *
 * @param color Text color
 * @return ABaseUI& A reference to this UI element
 */
ABaseUI &	ABaseUI::setTextColor(glm::vec4 color) { _textColor = color; return *this; }
/**
 * @brief Set the text font
 *
 * @param font The text font
 * @return ABaseUI& A reference to this UI element
 */
ABaseUI &	ABaseUI::setTextFont(std::string const & font) { _textFont = font; return *this; }
/**
 * @brief Set the text scale
 *
 * @param scale The text scale
 * @return ABaseUI& A reference to this UI element
 */
ABaseUI &	ABaseUI::setTextScale(float scale) { _textScale = scale; return *this; }
/**
 * @brief Set the text padding
 *
 * @param padding the text padding
 * @return ABaseUI& A reference to this UI element
 */
ABaseUI &	ABaseUI::setTextPadding(float padding) { _textPadding = padding; return *this; }
/**
 * @brief Set the text alignment
 *
 * @param align The text alignment
 * @return ABaseUI& A reference to this UI element
 */
ABaseUI &	ABaseUI::setTextAlign(TextAlign::Enum align) { _textAlign = align; return *this; }
/**
 * @brief Set the text outline
 *
 * @param outline The text outline
 * @return ABaseUI& A reference to this UI element
 */
ABaseUI &	ABaseUI::setTextOutline(float outline) { _textOutline = outline; return *this; }
/**
 * @brief Set the text outline color
 *
 * @param color Text outline color
 * @return ABaseUI& A reference to this UI element
 */
ABaseUI &	ABaseUI::setTextOutlineColor(glm::vec4 color) { _textOutlineColor = color; return *this; }

/**
 * @brief Set the master object
 *
 * @param master The master object
 * @return ABaseUI& A reference to this UI element
 */
ABaseUI &	ABaseUI::setMaster(ABaseMasterUI * master) {
	if (_master != nullptr)
		_master->removeChild(this);
	_master = master;
	if (_master != nullptr)
		_master->addChild(this);
	return *this;
}

/* getter */
/**
 * @brief Get mouse hover state
 *
 * @return true If mouse hover
 */
bool				ABaseUI::getMouseHover() const { return _mouseHover; }
/**
 * @brief Get mouse right click
 *
 * @return true If mouse right is clicked
 */
bool				ABaseUI::getMouseRightClick() const { return _rightClick; }
/**
 * @brief Get mouse left click
 *
 * @return true If mouse left clicked
 */
bool				ABaseUI::getMouseLeftClick() const { return _leftClick; }

/**
 * @brief Know if the element is enabled
 *
 * @return true If enabled
 */
bool				ABaseUI::isEnabled() const { return _enabled; }
/**
 * @brief Get the position
 *
 * @return glm::vec2& The position
 */
glm::vec2 &			ABaseUI::getPos() { return _pos; }
/**
 * @brief Get tje position
 *
 * @return glm::vec2 const& The position
 */
glm::vec2 const &	ABaseUI::getPos() const { return _pos; }
/**
 * @brief Get the z position
 *
 * @return float the z position
 */
float				ABaseUI::getZ() const { return _z; }
/**
 * @brief Get the ui color
 *
 * @return glm::vec4 the ui color in rgba order
 */
glm::vec4			ABaseUI::getColor() const { return _color; }
/**
 * @brief Get the real position (position + master position + offset)
 *
 * @return glm::vec2 The real position
 */
glm::vec2			ABaseUI::getRealPos() const {
	glm::vec2 masterPos = (_master) ? _master->getMasterRealPos() : glm::vec2(0, 0);
	return masterPos + _pos + _posOffset;
}
/**
 * @brief Get the size
 *
 * @return glm::vec2& The size
 */
glm::vec2 &			ABaseUI::getSize() { return _size; }
/**
 * @brief Get the size
 *
 * @return glm::vec2 const& The size
 */
glm::vec2 const &	ABaseUI::getSize() const { return _size; }
/**
 * @brief Get the rectangle Shader
 *
 * @return Shader& The rectangle shader
 */
Shader &			ABaseUI::getRectShader() { return *_rectShader; }
/**
 * @brief Get the text width
 *
 * @return uint32_t The text width
 */
uint32_t			ABaseUI::getTextWidth() const {
	return _textRender->strWidth(_textFont, _text, _textScale) + _textPadding * 2;
}
/**
 * @brief Get the text
 *
 * @return std::string The text
 */
std::string			ABaseUI::getText() const { return _text; }
/**
 * @brief Get the default image size
 *
 * @return glm::ivec2& The def image size
 */
glm::ivec2 &		ABaseUI::getImgDefSize() { return _imgDefSize; }
/**
 * @brief Get the default image size
 *
 * @return glm::ivec2 const& The def image size
 */
glm::ivec2 const &	ABaseUI::getImgDefSize() const { return _imgDefSize; }

// -- exception ----------------------------------------------------------------
ABaseUI::UIException::UIException()
: std::runtime_error("UI Exception") {}

ABaseUI::UIException::UIException(const char* what_arg)
: std::runtime_error(std::string(std::string("UIError: ") + what_arg).c_str()) {}
