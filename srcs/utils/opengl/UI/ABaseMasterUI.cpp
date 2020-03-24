#include "ABaseMasterUI.hpp"
#include "Logging.hpp"
#include "debug.hpp"

ABaseMasterUI::ABaseMasterUI(glm::vec2 pos, glm::vec2 size): ABaseUI(pos, size) {
}

ABaseMasterUI::ABaseMasterUI(ABaseMasterUI const & src): ABaseUI(src) {
	*this = src;
}

ABaseMasterUI::~ABaseMasterUI() {}

ABaseMasterUI & ABaseMasterUI::operator=(ABaseMasterUI const & rhs) {
	(void)rhs;
	// if (this != &rhs) {}
	return *this;
}

glm::vec2 ABaseMasterUI::getMasterPos() const {
	return getRealPos() + _borderSize;
}
glm::vec2 ABaseMasterUI::getMasterSize() const {
	glm::vec2 size = _size;
	size.x -= 2 * _borderSize;
	size.y -= 2 * _borderSize;
	return size;
}
