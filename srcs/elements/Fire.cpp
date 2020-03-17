#include "Fire.hpp"
#include "SceneGame.hpp"
#include "Player.hpp"

// -- Constructors -------------------------------------------------------------

Fire::Fire(SceneGame &game) : AObject(game) {
	type = Type::FIRE;
	name = "Fire";
	blockPropagation = false;
	destructible = true;
	crossable = Type::ALL;
	_timeToDie = 1.0f;
}

Fire::~Fire() {
	getPos();
	game.clearFromBoard(this, {position.x, position.z});
}

Fire::Fire(Fire const &src) : AObject(src) {
	*this = src;
}

// -- Operators ----------------------------------------------------------------

Fire &Fire::operator=(Fire const &rhs) {
	if ( this != &rhs ) {
		AObject::operator=(rhs);
	}
	return *this;
}

// -- Methods ------------------------------------------------------------------

/**
 * @brief update is called each frame.
 *
 * @param dTime Delta Time
 * @return true if success
 * @return false if failure
 */
bool	Fire::update(float const dTime) {
	_timeToDie -= dTime;
	if (_timeToDie <= 0.0) {
		alive = false;
	}
	getPos();
	for (auto &&enemy : game.enemies) {
		if (enemy->hasCollision(position, 0.2f)) {
			enemy->takeDamage(1);
		}
	}
	if (game.player->hasCollision(position, 0.2f)) {
		game.player->takeDamage(1);
	}
	return true;
}

bool	Fire::postUpdate() {
	if (!alive) {
		delete this;
	}
	return true;
}

/**
 * @brief draw is called each frame.
 *
 * @return true if success
 * @return false if failure
 */
bool	Fire::draw(Gui &gui) {
	gui.drawCube(Block::FIRE, getPos());
	return true;
}


// -- Exceptions errors --------------------------------------------------------

Fire::FireException::FireException()
: std::runtime_error("Fire Exception") {}

Fire::FireException::FireException(const char* whatArg)
: std::runtime_error(std::string(std::string("FireError: ") + whatArg).c_str()) {}
