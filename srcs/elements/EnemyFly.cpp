#include "EnemyFly.hpp"
#include "Player.hpp"

// -- Constructors -------------------------------------------------------------

EnemyFly::EnemyFly(SceneGame &game)
: AEnemy(game)
{
	name = "EnemyFly";
	size = glm::vec3(0.5, 0.5, 0.3);
}

EnemyFly::~EnemyFly() {
	auto it = game.enemies.begin();
	while (it != game.enemies.end()) {
		if ((*it) == this)
			game.enemies.erase(it);
		else
			it++;
	}
}

EnemyFly::EnemyFly(EnemyFly const &src) : AEnemy(src) {
	*this = src;
}

// -- Operators ----------------------------------------------------------------

EnemyFly &EnemyFly::operator=(EnemyFly const &rhs) {
	if ( this != &rhs ) {
		logWarn("EnemyFly operator= called (not implemented)");
		AEnemy::operator=(rhs);
	}
	return *this;
}

// -- Methods ------------------------------------------------------------------

/**
 * @brief update is called each frame.
 *
 * @return true if success
 * @return false if failure
 */
bool	EnemyFly::_update() {
	if (_isBlocked())  // do nothing if blocked
		return true;
	_baseEnemyMove(_dir);
	return true;
}

/**
 * @brief postUpdate is called each frame. After update()
 *
 * @return true if success
 * @return false if failure
 */
bool	EnemyFly::_postUpdate() {
	return true;
}

/**
 * @brief draw is called each frame.
 *
 * @return true if success
 * @return false if failure
 */
bool	EnemyFly::_draw(Gui &gui) {
	gui.drawCube(Block::IA, getPos(), size);
	return true;
}
