#include "SliderUI.hpp"
#include "Logging.hpp"
#include "debug.hpp"
#include "Inputs.hpp"

/**
 * @brief Construct a new Slider U I:: Slider U I object
 *
 * @param pos The position
 * @param size The size
 */
SliderUI::SliderUI(glm::vec2 pos, glm::vec2 size)
: ABaseUI(pos, size),
  _min(0),
  _max(1),
  _val(0.5),
  _step(0),
  _sliderListener(nullptr)
{}

/**
 * @brief Construct a new Slider U I:: Slider U I object
 *
 * @param src The object to do the copy
 */
SliderUI::SliderUI(SliderUI const & src): ABaseUI(src) {
	*this = src;
}

/**
 * @brief Destroy the Slider U I:: Slider U I object
 */
SliderUI::~SliderUI() {}

/**
 * @brief Copy this object
 *
 * @param rhs The object to copy
 * @return SliderUI& A reference to the copied object
 */
SliderUI & SliderUI::operator=(SliderUI const & rhs) {
	(void)rhs;
	// if (this != &rhs) {}
	return *this;
}

/**
 * @brief Set Slider values
 *
 * @param min Min value
 * @param max Max value
 * @param val Start value
 * @param step Step btw 2 values
 */
void SliderUI::setValues(float min, float max, float val, float step) {
	_min = min;
	_max = max;
	_val = val;
	_step = step;
}

/**
 * @brief Set the current value
 *
 * @param val Value
 */
void SliderUI::setValue(float val) {
	if (val < _min)
		_val = _min;
	else if (val > _max)
		_val = _max;
	else
		_val = val;
}

/**
 * @brief this is the base update function of UI objects
 */
void SliderUI::_update() {
	glm::vec2 mousePos = Inputs::getMousePos();
	if (Inputs::getLeftClickUp() && _leftListener) {
		*_leftListener = true;
	}
	if (_leftClick) {
		// get a factor (btw 0 & 1)
		float factor = (mousePos.x - getRealPos().x - _borderSize) / (_size.x - _borderSize * 2);
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

/**
 * @brief this is the draw function for UI
 * /!\ -> you need to draw in the reverse order (draw at first the element on the top)
 */
void SliderUI::_draw() {
	glm::vec2 tmpPos;
	glm::vec2 tmpSize;

	// draw text
	tmpPos = getRealPos();
	tmpPos.x += _borderSize;
	tmpSize = _size;
	tmpSize.x -= _borderSize * 2;
	std::string valStr = std::to_string(_val);
	if (_val - static_cast<int>(_val) < 0.1 || _val - static_cast<int>(_val) > 0.9)
		valStr = std::to_string(static_cast<int>(_val));
	_drawText(tmpPos, tmpSize, _z, _textFont, _textScale, valStr, _textColor, _textAlign, _textPadding);

	// get center size and position
	tmpPos = getRealPos();
	tmpSize = _size;

	tmpPos.y += _borderSize;
	tmpSize.y -= _borderSize * 2;

	float width = (_size.x - (_borderSize * 2)) * ((_val - _min) / _max);
	tmpPos.x += _borderSize;
	tmpSize.x = width;

    // draw center
	_drawRect(tmpPos, tmpSize, _z, _color);

	// draw border
	_drawBorderRect(getRealPos(), _size, _z, _borderSize, _borderColor);
}

/* listener */
/**
 * @brief add a listener on the slider
 *
 * @param listener a pointer on the float listener
 * @return SliderUI& a reference to the object
 */
SliderUI &	SliderUI::addSliderListener(float * listener) {
	_sliderListener = listener;
	*_sliderListener = _val;
	return *this;
}
