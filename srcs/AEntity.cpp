#include "AEntity.hpp"

// -- Constructors -------------------------------------------------------------

AEntity::AEntity() {
}

AEntity::~AEntity() {
}

AEntity::AEntity(AEntity const &src) {
	*this = src;
}

// -- Operators ----------------------------------------------------------------

AEntity &AEntity::operator=(AEntity const &rhs) {
	if ( this != &rhs ) {
		pos = rhs.pos;
		category = rhs.category;
		name = rhs.name;
		type = rhs.type;
		active = rhs.active;
	}
	return *this;
}
