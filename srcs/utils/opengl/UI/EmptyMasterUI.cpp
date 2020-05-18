#include "EmptyMasterUI.hpp"
#include "Logging.hpp"
#include "debug.hpp"

/**
 * @brief Construct a new Empty Master U I:: Empty Master U I object
 *
 * @param pos The position of the UI element
 * @param size The size of the UI element
 */
EmptyMasterUI::EmptyMasterUI(glm::vec2 pos, glm::vec2 size)
: ABaseMasterUI(pos, size) {}

/**
 * @brief Construct a new Empty Master U I:: Empty Master U I object
 *
 * @param src The object to do the copy
 */
EmptyMasterUI::EmptyMasterUI(EmptyMasterUI const & src): ABaseMasterUI(src) {
	*this = src;
}

/**
 * @brief Destroy the Empty Master U I:: Empty Master U I object
 */
EmptyMasterUI::~EmptyMasterUI() {}

/**
 * @brief Copy this object
 *
 * @param rhs The object to copy
 * @return EmptyMasterUI& A reference to the copied object
 */
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
