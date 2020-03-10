#include "AEntity.hpp"
#include "SceneGame.hpp"

// -- Constructors -------------------------------------------------------------

AEntity::AEntity(SceneGame &game): game(game) {
	active = true;
	alive = true;
	pos = VOID_POS3;
	category = Category::STATIC;
	name = "Entity";
}

AEntity::~AEntity() {
}

AEntity::AEntity(AEntity const &src) : AEntity(src.game) {
	*this = src;
}

// -- Operators ----------------------------------------------------------------

AEntity &AEntity::operator=(AEntity const &rhs) {
	if ( this != &rhs ) {
		category = rhs.category;
		name = rhs.name;
		type = rhs.type;
		active = rhs.active;
	}
	return *this;
}
