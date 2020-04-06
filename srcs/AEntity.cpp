#include "AEntity.hpp"
#include "SceneGame.hpp"
#include "Model.hpp"

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
	_model = nullptr;
	_animDeathEnd = false;
}

AEntity::~AEntity() {
	delete _model;
}

AEntity::AEntity(AEntity const &src) : AEntity(src.game) {
	*this = src;
}

// -- Methods ------------------------------------------------------------------

/**
 * @brief Init method called by loadLevel
 *
 * @return true on success
 * @return false on failure
 */
bool		AEntity::init() {
	return true;
}

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
		_model = nullptr;
		_animDeathEnd = false;

		// if exist, copy the model
		if (rhs._model) {
			init();  // create new model
			*_model = *rhs._model;  // restore model settings
		}
	}
	return *this;
}
