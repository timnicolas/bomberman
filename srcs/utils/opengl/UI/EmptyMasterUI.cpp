#include "EmptyMasterUI.hpp"
#include "Logging.hpp"
#include "debug.hpp"

EmptyMasterUI::EmptyMasterUI(glm::vec2 pos, glm::vec2 size)
: ABaseMasterUI(pos, size) {}

EmptyMasterUI::EmptyMasterUI(EmptyMasterUI const & src): ABaseMasterUI(src) {
	*this = src;
}

EmptyMasterUI::~EmptyMasterUI() {}

EmptyMasterUI & EmptyMasterUI::operator=(EmptyMasterUI const & rhs) {
	(void)rhs;
	// if (this != &rhs) {}
	return *this;
}

/**
 * @brief this is the base update function of UI objects
 */
void EmptyMasterUI::_update() {
	/* update master size */
	_updateTotalMasterSize();
}

/**
 * @brief this is the draw function for UI
 * /!\ -> you need to draw in the reverse order (draw at first the element on the top)
 */
void EmptyMasterUI::_draw() {}
