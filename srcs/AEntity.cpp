#include "AEntity.hpp"
#include "SceneGame.hpp"

// -- Constructors -------------------------------------------------------------

AEntity::AEntity(SceneGame &game): game(game) {
	active = true;
	alive = true;
	position = VOID_POS3;
	category = Category::STATIC;
	name = "Entity";
	crossable = Type::NONE;
	_timeToDie = 0.5f;
}

AEntity::~AEntity() {
}

AEntity::AEntity(AEntity const &src) : AEntity(src.game) {
	*this = src;
}

// -- Methods ------------------------------------------------------------------

bool		AEntity::postUpdate() {
	return true;
}

// -- Operators ----------------------------------------------------------------

AEntity &AEntity::operator=(AEntity const &rhs) {
	if ( this != &rhs ) {
		active = rhs.active;
		alive = rhs.alive;
		category = rhs.category;
		name = rhs.name;
		type = rhs.type;
		game = rhs.game;
		destructible = rhs.destructible;
		blockPropagation = rhs.blockPropagation;
		crossable = rhs.crossable;
		position = rhs.position;
		_timeToDie = rhs._timeToDie;
	}
	return *this;
}
