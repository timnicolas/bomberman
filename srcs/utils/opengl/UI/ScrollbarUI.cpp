#include "ScrollbarUI.hpp"
#include "Logging.hpp"
#include "debug.hpp"

/**
 * @brief Construct a new Scrollbar U I:: Scrollbar U I object
 *
 * @param pos The position of the UI element
 * @param size The size of the UI element
 */
ScrollbarUI::ScrollbarUI(glm::vec2 pos, glm::vec2 size)
: ABaseMasterUI(pos, size),
  _scrollbarSize(20),
  _scrollbarColor(0.3, 0.3, 0.3, 1.0),
  _mouseScrollSpeed(15),

  _vertScroll(false),
  _vertScrollInverted(false),
  _vertScrollbarPos(0),
  _vertScrollBarDrawSize(0),
  _isVertScrollClicked(false),

  _horizScroll(false),
  _horizScrollInverted(false),
  _horizScrollbarPos(0),
  _horizScrollBarDrawSize(0),
  _isHorizScrollClicked(false)
{
	setColor(glm::vec4(0.0, 0.0, 0.0, 0.0));
}

/**
 * @brief Construct a new Scrollbar U I:: Scrollbar U I object
 *
 * @param src The object to do the copy
 */
ScrollbarUI::ScrollbarUI(ScrollbarUI const & src): ABaseMasterUI(src) {
	*this = src;
}

/**
 * @brief Destroy the Scrollbar U I:: Scrollbar U I object
 */
ScrollbarUI::~ScrollbarUI() {}

/**
 * @brief Copy this object
 *
 * @param rhs The object to copy
 * @return ScrollbarUI& A reference to the copied object
 */
ScrollbarUI & ScrollbarUI::operator=(ScrollbarUI const & rhs) {
	(void)rhs;
	// if (this != &rhs) {}
	return *this;
}

/**
 * @brief this is the base update function of UI objects
 */
void ScrollbarUI::_update() {
	/* update master size */
	_updateTotalMasterSize();

	/* get mouse position */
	glm::vec2 mousePos = Inputs::getMousePos();
	mousePos.y = _winSize.y - mousePos.y;
	glm::vec2 mouseScroll = Inputs::getMouseScroll();

	if (_vertScroll) {
		/* size of the slider */
		float scrollSizeFactor = getMasterSize().y / _masterTotalSize.y;
		if (scrollSizeFactor < 0.1) scrollSizeFactor = 0.1;
		if (scrollSizeFactor > 1) scrollSizeFactor = 1;
		_vertScrollBarDrawSize = (_size.y - _borderSize * 2) * scrollSizeFactor;

		/* show or hide scrollbar */
		float scrollbarHideFactor = getMasterSize().y / (_masterTotalSize.y - _masterPadding.y * 2);
		if (scrollbarHideFactor > 1) scrollbarHideFactor = 1;
		_vertScrollHide = false;
		if (scrollbarHideFactor == 1)
			_vertScrollHide = true;

		if (!_vertScrollHide) {
			// if click on the scroll zone
			if (_leftClick) {
				if (mousePos.x >= getRealPos().x + _size.x - _borderSize - _scrollbarSize
				&& mousePos.x <= getRealPos().x + _size.x - _borderSize
				&& mousePos.y >= getRealPos().y + _borderSize
				&& mousePos.y <= getRealPos().y + _size.y - _borderSize)
				{
					_isVertScrollClicked = true;
				}
				if (_isVertScrollClicked) {
					// float yMin = getRealPos().y + _borderSize;
					// float yMax = getRealPos().y + _size.y - (_borderSize * 2);
					float yMin = getRealPos().y + _borderSize + _vertScrollBarDrawSize / 2;
					float yMax = getRealPos().y + _size.y - (_borderSize * 2 + _vertScrollBarDrawSize / 2);
					if (mousePos.y < yMin)
						_vertScrollbarPos = 1;
					else if (mousePos.y > yMax)
						_vertScrollbarPos = 0;
					else
						_vertScrollbarPos = 1 - (mousePos.y - yMin) / (yMax - yMin);
				}
			}
			else {
				_isVertScrollClicked = false;
			}
			if (_mouseHover) {
				// if scrolling
				if (mouseScroll.y != 0) {
					float offset = mouseScroll.y * _mouseScrollSpeed * (1 / _masterTotalSize.y);
					if (_vertScrollInverted)
						_vertScrollbarPos += offset;
					else
						_vertScrollbarPos -= offset;
					if (_vertScrollbarPos < 0) _vertScrollbarPos = 0;
					if (_vertScrollbarPos > 1) _vertScrollbarPos = 1;
				}
			}
			_masterOffset.y = (_masterTotalSize.y - (_size.y - _borderSize * 2)) * _vertScrollbarPos;
		}
	}

	if (_horizScroll) {
		/* size of the slider */
		float scrollSizeFactor = getMasterSize().x / _masterTotalSize.x;
		if (scrollSizeFactor < 0.1) scrollSizeFactor = 0.1;
		if (scrollSizeFactor > 1) scrollSizeFactor = 1;
		_horizScrollBarDrawSize = (_size.x - _borderSize * 2) * scrollSizeFactor;

		/* show or hide scrollbar */
		float scrollbarHideFactor = getMasterSize().x / (_masterTotalSize.x - _masterPadding.x * 2);
		if (scrollbarHideFactor > 1) scrollbarHideFactor = 1;
		_horizScrollHide = false;
		if (scrollbarHideFactor == 1)
			_horizScrollHide = true;

		if (!_horizScrollHide) {
			// if click on the scroll zone
			if (_leftClick) {
				if (mousePos.y >= getRealPos().y + _borderSize
				&& mousePos.y <= getRealPos().y + _borderSize + _scrollbarSize
				&& mousePos.x >= getRealPos().x + _borderSize
				&& mousePos.x <= getRealPos().x + _size.x - _borderSize)
				{
					_isHorizScrollClicked = true;
				}
				if (_isHorizScrollClicked) {
					// float xMin = getRealPos().x + _borderSize;
					// float xMax = getRealPos().x + _size.x - (_borderSize * 2);
					float xMin = getRealPos().x + _borderSize + _horizScrollBarDrawSize / 2;
					float xMax = getRealPos().x + _size.x - (_borderSize * 2 + _horizScrollBarDrawSize / 2);
					if (mousePos.x < xMin)
						_horizScrollbarPos = 0;
					else if (mousePos.x > xMax)
						_horizScrollbarPos = 1;
					else
						_horizScrollbarPos = (mousePos.x - xMin) / (xMax - xMin);
				}
			}
			else {
				_isHorizScrollClicked = false;
			}
			if (_mouseHover) {
				// if scrolling
				if (mouseScroll.x != 0) {
					float offset = mouseScroll.x * _mouseScrollSpeed * (1 / _masterTotalSize.x);
					if (_horizScrollInverted)
						_horizScrollbarPos -= offset;
					else
						_horizScrollbarPos += offset;
					if (_horizScrollbarPos < 0) _horizScrollbarPos = 0;
					if (_horizScrollbarPos > 1) _horizScrollbarPos = 1;
				}
			}
			_masterOffset.x = -1 * (_masterTotalSize.x - (_size.x - _borderSize * 2)) * _horizScrollbarPos;
		}
	}
}

/**
 * @brief this is the draw function for UI
 * /!\ -> you need to draw in the reverse order (draw at first the element on the top)
 */
void ScrollbarUI::_draw() {
	glm::vec2 tmpPos;
	glm::vec2 tmpSize;

	// draw scrollbars
	if (_vertScroll && !_vertScrollHide) {
		/* size of the scrollbar */
		tmpSize.x = _scrollbarSize;
		tmpSize.y = _vertScrollBarDrawSize;  // height depend to the total master size

		/* position of the scrollbar */
		tmpPos.x = getRealPos().x + _size.x - _borderSize - tmpSize.x;

		float y = (1 - _vertScrollbarPos) * ((_size.y - _borderSize * 2) - _vertScrollBarDrawSize);
		if (y < 0) y = 0;
		if (y > (_size.y - _borderSize * 2) - tmpSize.y) y = (_size.y - _borderSize * 2) - tmpSize.y;

		tmpPos.x = getRealPos().x + _size.x - _borderSize - _scrollbarSize;
		tmpPos.y = getRealPos().y + _borderSize + y;

		/* clicked color */
		float factor = 1;
		glm::vec4 secColor = glm::vec4(0.0, 0.0, 0.0, 0.0);
		if (_isVertScrollClicked) {
			secColor = _mouseHoverColor;
			factor = 1 - secColor.a;
			secColor.a = 1;
		}

		/* draw scrollbar */
		_drawRect(tmpPos, tmpSize, _z, _scrollbarColor, secColor, factor);
	}
	if (_horizScroll && !_horizScrollHide) {
		/* size of the scrollbar */
		tmpSize.y = _scrollbarSize;
		tmpSize.x = _horizScrollBarDrawSize;  // height depend to the total master size

		/* position of the scrollbar */
		tmpPos.y = getRealPos().y + _borderSize;

		float x = _horizScrollbarPos * ((_size.x - _borderSize * 2) - _horizScrollBarDrawSize);
		if (x < 0) x = 0;
		if (x > (_size.x - _borderSize * 2) - tmpSize.x) x = (_size.x - _borderSize * 2) - tmpSize.x;

		tmpPos.x = getRealPos().x + _size.x - _borderSize - _scrollbarSize;
		tmpPos.x = getRealPos().x + _borderSize + x;

		/* clicked color */
		float factor = 1;
		glm::vec4 secColor = glm::vec4(0.0, 0.0, 0.0, 0.0);
		if (_isHorizScrollClicked) {
			secColor = _mouseHoverColor;
			factor = 1 - secColor.a;
			secColor.a = 1;
		}

		/* draw scrollbar */
		_drawRect(tmpPos, tmpSize, _z, _scrollbarColor, secColor, factor);
	}

	// get center size and position
	tmpPos = getRealPos();
	tmpPos.x += _borderSize;
	tmpPos.y += _borderSize;
	tmpSize = _size;
	tmpSize.x -= _borderSize * 2;
	tmpSize.y -= _borderSize * 2;
	_drawRect(tmpPos, tmpSize, _z, _color);

	// draw border
	_drawBorderRect(getRealPos(), _size, _z, _borderSize, _borderColor);
}

/* setter */
/**
 * @brief Enable vertical scrolling
 *
 * @param enable Enable / disable
 * @return ScrollbarUI& A reference to 'this' ScrollbarUI object
 */
ScrollbarUI &	ScrollbarUI::enableVertScroll(bool enable) { _vertScroll = enable; return *this; }
/**
 * @brief Enable horizontal scrolling
 *
 * @param enable Enable / disable
 * @return ScrollbarUI& A reference to 'this' ScrollbarUI object
 */
ScrollbarUI &	ScrollbarUI::enableHorizScroll(bool enable) { _horizScroll = enable; return *this; }
/**
 * @brief Set the scrollbar size (width)
 *
 * @param size The scrollbar size
 * @return ScrollbarUI& A reference to 'this' ScrollbarUI object
 */
ScrollbarUI &	ScrollbarUI::setScrollbarSize(float size) { _scrollbarSize = size; return *this; }
/**
 * @brief Set the scrolling speed
 *
 * @param speed The scrolling speed
 * @return ScrollbarUI& A reference to 'this' ScrollbarUI object
 */
ScrollbarUI &	ScrollbarUI::setScrollbarSpeed(float speed) { _mouseScrollSpeed = speed; return *this; }
/**
 * @brief Invert vertical scroll
 *
 * @param invert Invert
 * @return ScrollbarUI& A reference to 'this' ScrollbarUI object
 */
ScrollbarUI &	ScrollbarUI::invertVertScroll(bool invert) { _vertScrollInverted = invert; return *this; }
/**
 * @brief Invert horizontal scroll
 *
 * @param invert Invert
 * @return ScrollbarUI& A reference to 'this' ScrollbarUI object
 */
ScrollbarUI &	ScrollbarUI::invertHorizScroll(bool invert) { _horizScrollInverted = invert; return *this; }

/**
 * @brief Get position of master element (bottom left after border)
 *
 * !!! This functions can be overwrite in childs class
 *
 * @return glm::vec2 The position
 */
glm::vec2 ScrollbarUI::getMasterPos() const {
	glm::vec2 pos = getRealPos() + _borderSize;
	if (_horizScroll)
		pos.y += _scrollbarSize;
	return pos;
}
/**
 * @brief Get position of master element (bottom left after border) considering offset
 *
 * !!! This functions can be overwrite in childs class
 *
 * @return glm::vec2 The position
 */
glm::vec2 ScrollbarUI::getMasterRealPos() const {
	glm::vec2 pos = getRealPos() + _masterOffset + _borderSize;
	if (_horizScroll)
		pos.y += _scrollbarSize;
	return pos;
}
/**
 * @brief Get size of master element inside the borders
 *
 * !!! This functions can be overwrite in childs class
 *
 * @return glm::vec2 The size
 */
glm::vec2 ScrollbarUI::getMasterSize() const {
	glm::vec2 size = _size;
	size.x -= 2 * _borderSize;
	if (_vertScroll)
		size.x -= _scrollbarSize;
	size.y -= 2 * _borderSize;
	if (_horizScroll)
		size.y -= _scrollbarSize;
	return size;
}
