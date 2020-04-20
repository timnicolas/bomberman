#include "ABaseMasterUI.hpp"
#include "Logging.hpp"
#include "debug.hpp"

ABaseMasterUI::ABaseMasterUI(glm::vec2 pos, glm::vec2 size)
: ABaseUI(pos, size),
  _masterPadding(10),
  _masterOffset(0, 0),
  _masterMinPos(0, 0),
  _masterMaxPos(0, 0),
  _masterTotalSize(0, 0)
{}

ABaseMasterUI::ABaseMasterUI(ABaseMasterUI const & src): ABaseUI(src) {
	*this = src;
}

ABaseMasterUI::~ABaseMasterUI() {
	// remove master for all childs
	while (!_childUI.empty()) {
		_childUI[0]->setMaster(nullptr);
	}
}

ABaseMasterUI & ABaseMasterUI::operator=(ABaseMasterUI const & rhs) {
	(void)rhs;
	// if (this != &rhs) {}
	return *this;
}

/* setter */
/**
 * @brief Add a child in master element (called by setMaster)
 *
 * @param child The child element
 */
void ABaseMasterUI::addChild(ABaseUI * child) {
	_childUI.push_back(child);
}
/**
 * @brief Remove a child in master element (called by setMaster)
 *
 * @param child The child element
 */
void ABaseMasterUI::removeChild(ABaseUI * child) {
	auto it = std::find(_childUI.begin(), _childUI.end(), child);
	if (it != _childUI.end())
		_childUI.erase(it);
	else
		logWarn("in ABaseMasterUI::removeChild(), child is not in master");
}

/* getter */
/**
 * @brief Get position of master element (bottom left after border)
 *
 * !!! This functions can be overwrite in childs class
 *
 * @return glm::vec2 The position
 */
glm::vec2 ABaseMasterUI::getMasterPos() const {
	return getRealPos() + _borderSize;
}
/**
 * @brief Get position of master element (bottom left after border) considering offset
 *
 * !!! This functions can be overwrite in childs class
 *
 * @return glm::vec2 The position
 */
glm::vec2 ABaseMasterUI::getMasterRealPos() const {
	return getRealPos() + _masterOffset + _borderSize;
}
/**
 * @brief Get size of master element inside the borders
 *
 * !!! This functions can be overwrite in childs class
 *
 * @return glm::vec2 The size
 */
glm::vec2 ABaseMasterUI::getMasterSize() const {
	glm::vec2 size = _size;
	size.x -= 2 * _borderSize;
	size.y -= 2 * _borderSize;
	return size;
}
/**
 * @brief Get the total size of all element in master
 *
 * this function update:
 *  - _masterMinPos: The minimum object position in object
 *  - _masterMaxPos: The maximum object position in object
 *  - _masterTotalSize: The total size of master elements
 */
void ABaseMasterUI::_updateTotalMasterSize() {
	_masterMinPos = glm::vec2(0, 0);
	_masterMaxPos = glm::vec2(0, 0);

	for (auto && it : _childUI) {
		glm::vec2 tmpPos1 = it->getRealPos() - getMasterRealPos();
		glm::vec2 tmpPos2 = tmpPos1 + it->getSize();
		if (tmpPos2.x > _masterMaxPos.x)
			_masterMaxPos.x = tmpPos2.x;
		if (tmpPos1.x < _masterMinPos.x)
			_masterMinPos.x = tmpPos1.x;
		if (tmpPos2.y > _masterMaxPos.y)
			_masterMaxPos.y = tmpPos2.y;
		if (tmpPos1.y < _masterMinPos.y)
			_masterMinPos.y = tmpPos1.y;
	}
	_masterMinPos -= _masterPadding;
	_masterMaxPos += _masterPadding;
	_masterTotalSize = _masterMaxPos - _masterMinPos;
}
