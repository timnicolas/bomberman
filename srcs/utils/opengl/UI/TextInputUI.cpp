#include <cctype>
#include "TextInputUI.hpp"
#include "Logging.hpp"
#include "debug.hpp"

TextInputUI::TextInputUI(glm::vec2 pos, glm::vec2 size)
: ABaseUI(pos, size),
  _defText(""),
  _defTextColor(0.3, 0.3, 0.3, 1),
  _ignoredKeys{
	  SDL_SCANCODE_LSHIFT,
	  SDL_SCANCODE_RSHIFT,
	  SDL_SCANCODE_LCTRL,
	  SDL_SCANCODE_RCTRL,
  }
{
	setColor(glm::vec4(0.0, 0.0, 0.0, 0.0));
	setBorderSize(0);
}

TextInputUI::TextInputUI(TextInputUI const & src): ABaseUI(src) {
	*this = src;
}

TextInputUI::~TextInputUI() {}

TextInputUI & TextInputUI::operator=(TextInputUI const & rhs) {
	(void)rhs;
	// if (this != &rhs) {}
	return *this;
}

/**
 * @brief this is the base update function of UI objects
 */
void TextInputUI::_update() {
	SDL_Scancode scan = Inputs::getTextInputScancode();
	bool ingoreScan = (std::find(_ignoredKeys.begin(), _ignoredKeys.end(), scan) != _ignoredKeys.end());
	if (scan != NO_SCANCODE && !ingoreScan) {  // if a valid key was pressed
		std::string scanStr = "";
		bool uppercase = false;
		if (Inputs::getKeyByScancode(SDL_SCANCODE_LSHIFT) || Inputs::getKeyByScancode(SDL_SCANCODE_RSHIFT)) {
			uppercase = true;
		}
		switch (scan)
		{
			case SDL_SCANCODE_SPACE:
				scanStr = " ";
				break;
			case SDL_SCANCODE_TAB:
				scanStr = "\t";
				break;
			case SDL_SCANCODE_BACKSPACE:
				if (_text.size() > 0) {
					_text.pop_back();
				}
				break;
			default:
				scanStr = Inputs::getScancodeName(scan);
				break;
		}
		if (scanStr.size() == 1 && isalpha(scanStr[0])) {
			if (uppercase)
				scanStr = toupper(scanStr[0]);
			else
				scanStr = tolower(scanStr[0]);
		}
		_text += scanStr;
	}
}

/**
 * @brief this is the draw function for UI
 * /!\ -> you need to draw in the reverse order (draw at first the element on the top)
 */
void TextInputUI::_draw() {
	glm::vec2 tmpPos;
	glm::vec2 tmpSize;

	// draw text
	std::string textToPrint = (_text == "") ? _defText : _text;
	glm::vec4	textColor = (_text == "") ? _defTextColor : _textColor;
	tmpPos = getRealPos();
	tmpPos.x += _borderSize;
	tmpSize = _size;
	tmpSize.x -= _borderSize * 2;
	_drawText(tmpPos, tmpSize, _textFont, _textScale, textToPrint, textColor, _textAlign, _textPadding);

	// get center size and position
	tmpPos = getRealPos();
	tmpPos.x += _borderSize;
	tmpPos.y += _borderSize;
	tmpSize = _size;
	tmpSize.x -= _borderSize * 2;
	tmpSize.y -= _borderSize * 2;
	_drawRect(tmpPos, tmpSize, _color);

	// draw border
	_drawBorderRect(getRealPos(), _size, _borderSize, _borderColor);
}

TextInputUI & TextInputUI::setDefText(std::string const & defText) { _defText = defText; return *this; }
TextInputUI & TextInputUI::setDefTextColor(glm::vec4 color) { _defTextColor = color; return *this; }
