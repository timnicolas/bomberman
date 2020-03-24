#include "ABaseMasterUI.hpp"
#include "Logging.hpp"
#include "debug.hpp"

ABaseMasterUI::ABaseMasterUI(glm::vec2 pos, glm::vec2 size): ABaseUI(pos, size) {
}

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
 * @return glm::vec2 The position
 */
glm::vec2 ABaseMasterUI::getMasterPos() const {
	return getRealPos() + _borderSize;
}
/**
 * @brief Get size of master element inside the borders
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
 * @return glm::vec2 The total size
 */
glm::vec2 ABaseMasterUI::getTotalMasterSize() const {
	glm::vec2 maxSize = glm::vec2(0, 0);
	glm::vec2 minSize = glm::vec2(0, 0);

	for (auto && it : _childUI) {
		glm::vec2 tmpSize = it->getRealPos() - getMasterPos() + it->getSize();
		if (tmpSize.x > maxSize.x)
			maxSize.x = tmpSize.x;
		if (tmpSize.x < minSize.x)
			minSize.x = tmpSize.x;
		if (tmpSize.y > maxSize.y)
			maxSize.y = tmpSize.y;
		if (tmpSize.y < minSize.y)
			minSize.y = tmpSize.y;
	}
	return maxSize - minSize;
}
