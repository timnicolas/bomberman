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
	_entityStatus.status = EntityStatus::IDLE;
	_entityStatus.updated = false;
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
 * @brief update the entity status
 *
 * @param status the new status
 */
void	AEntity::setStatus(EntityStatus::Enum status) {
	_entityStatus.status = status;
	_entityStatus.updated = true;
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
		_entityStatus.status = rhs._entityStatus.status;
		_entityStatus.updated = rhs._entityStatus.updated;
	}
	return *this;
}
