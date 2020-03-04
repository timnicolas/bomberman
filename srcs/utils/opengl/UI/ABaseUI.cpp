#include "ABaseUI.hpp"

ABaseUI::ABaseUI(glm::vec2 pos, glm::vec2 size)
: _pos(pos),
  _size(size)
{
}

ABaseUI::ABaseUI(ABaseUI const & src) {
	*this = src;
}

ABaseUI::~ABaseUI() {
}

ABaseUI & ABaseUI::operator=(ABaseUI const & rhs) {
	if (this != &rhs) {
		_pos = rhs._pos;
		_size = rhs._size;
	}
	return *this;
}

/* getter */
glm::vec2 const &	ABaseUI::getPos() const { return _pos; }
glm::vec2 &			ABaseUI::getPos() { return _pos; }
glm::vec2 const &	ABaseUI::getSize() const { return _size; }
glm::vec2 &			ABaseUI::getSize() { return _size; }
