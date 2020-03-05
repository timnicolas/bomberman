#include "SliderUI.hpp"
#include "Logging.hpp"
#include "debug.hpp"

SliderUI::SliderUI(glm::vec2 winSize, glm::vec2 pos, glm::vec2 size)
: ABaseUI(winSize, pos, size),
  _min(0),
  _max(1),
  _val(0.5),
  _step(0),
  _sliderListener(nullptr)
{
}

SliderUI::SliderUI(SliderUI const & src)
: ABaseUI(src)
{
	*this = src;
}

SliderUI::~SliderUI() {
}

SliderUI & SliderUI::operator=(SliderUI const & rhs) {
	(void)rhs;
	// if (this != &rhs) {}
	return *this;
}

void SliderUI::setValues(float min, float max, float val, float step) {
	_min = min;
	_max = max;
	_val = val;
	_step = step;
}

void SliderUI::_update(glm::vec2 mousePos, bool rightClick, bool leftClick) {
	(void)mousePos;
	(void)rightClick;
	(void)leftClick;
	if (_leftClick) {
		// get a factor (btw 0 & 1)
		float factor = (mousePos.x - _pos.x - _borderSize) / (_size.x - _borderSize * 2);
		if (factor < 0) factor = 0;
		if (factor > 1) factor = 1;
		// get the value from the factor
		float tmpVal = _min + factor * (_max - _min);

		// process step (if needed)
		if (_step > 0){
			float adder = tmpVal - _min;
			adder = std::fmod(adder, _step);
			if (adder < _step / 2) {
				adder = -adder;
			}
			else {
				adder = _step - adder;
			}
			tmpVal += adder;
		}
		_val = tmpVal;
		if (_sliderListener)
			*_sliderListener = _val;
	}
}

/*
	this is the draw function for buttons
	/!\ -> you need to draw in the reverse order (draw at first the element on the top)
*/
void SliderUI::draw() {
	glm::vec2 tmpPos;
	glm::vec2 tmpSize;

	// draw text
	tmpPos = _pos;
	tmpPos.x += _borderSize;
	tmpSize = _size;
	tmpSize.x -= _borderSize * 2;
	std::string valStr = std::to_string(_val);
	if (_val - static_cast<int>(_val) < 0.1 || _val - static_cast<int>(_val) > 0.9)
		valStr = std::to_string(static_cast<int>(_val));
	_drawText(tmpPos, tmpSize, _textScale, valStr, _textColor, _textAlign, _textPadding);

	// get center size and position
	tmpPos = _pos;
	tmpSize = _size;

	tmpPos.y += _borderSize;
	tmpSize.y -= _borderSize * 2;

	float width = (_size.x - (_borderSize * 2)) * ((_val - _min) / _max);
	tmpPos.x += _borderSize;
	tmpSize.x = width;

    // draw center
	_drawRect(tmpPos, tmpSize, _color);

	// draw border
	tmpSize = _size;
	tmpSize.y = _borderSize;

	tmpPos = _pos;
	_drawRect(tmpPos, tmpSize, _borderColor);
	tmpPos = _pos;
	tmpPos.y += _size.y - _borderSize;
	_drawRect(tmpPos, tmpSize, _borderColor);

	tmpSize = _size;
	tmpSize.x = _borderSize;

	tmpPos = _pos;
	_drawRect(tmpPos, tmpSize, _borderColor);
	tmpPos = _pos;
	tmpPos.x += _size.x - _borderSize;
	_drawRect(tmpPos, tmpSize, _borderColor);
}

/* listener */
SliderUI &	SliderUI::addSliderListener(float * listener) {
	_sliderListener = listener;
	*_sliderListener = _val;
	return *this;
}
