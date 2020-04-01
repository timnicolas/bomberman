#include "AEntity.hpp"
#include "SceneGame.hpp"

// -- Constructors -------------------------------------------------------------

AEntity::AEntity(SceneGame &game): game(game) {
	active = true;
	alive = true;
	position = VOID_POS3;
	category = Category::STATIC;
	name = "Entity";
	_timeToDie = 0.5f;
	_entityState.state = EntityState::IDLE;
	_entityState.updated = false;
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

/**
 * @brief called on animation end if passed to Model
 * need to be redefined by children
 *
 * @param animName the current animation name
 */
void	AEntity::animEndCb(std::string animName) {
	(void)animName;
	return;
}

/**
 * @brief update the entity state
 *
 * @param state the new state
 */
void	AEntity::setstate(EntityState::Enum state) {
	_entityState.state = state;
	_entityState.updated = true;
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
		position = rhs.position;
		_timeToDie = rhs._timeToDie;
		_entityState.state = rhs._entityState.state;
		_entityState.updated = rhs._entityState.updated;
	}
	return *this;
}
