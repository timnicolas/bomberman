#include "AObject.hpp"
#include "SceneGame.hpp"

// -- Constructors -------------------------------------------------------------

AObject::AObject(SceneGame &game) : AEntity(game) {
	category = Category::STATIC;
}

AObject::~AObject() {
}

AObject::AObject(AObject const &src) : AEntity(src) {
	*this = src;
}

// -- Operators ----------------------------------------------------------------

AObject &AObject::operator=(AObject const &rhs) {
	if ( this != &rhs ) {
		AEntity::operator=(rhs);
	}
	return *this;
}

// -- Methods ------------------------------------------------------------------

glm::vec2	AObject::getPos() {
	uint8_t i = 0;
	for (auto &&board_it0 : game.board) {
		uint8_t j = 0;
		for (auto &&board_it1 : board_it0) {
			for (AEntity *board_it2 : board_it1) {
				if (board_it2 == this)
					return {i, j};
			}
			j++;
		}
		i++;
	}
	throw AObjectException("Impossible to found the position of the object");
}

// -- Exceptions errors --------------------------------------------------------

AObject::AObjectException::AObjectException()
: std::runtime_error("AObject Exception") {}

AObject::AObjectException::AObjectException(const char* what_arg)
: std::runtime_error(std::string(std::string("AObjectError: ") + what_arg).c_str()) {}
