#include "EnemyBasic.hpp"
#include "Player.hpp"

// -- Constructors -------------------------------------------------------------

EnemyBasic::EnemyBasic(SceneGame &game)
: AEnemy(game),
  _dir(Direction::UP)
{
	name = "EnemyBasic";
}

EnemyBasic::~EnemyBasic() {
	auto it = game.enemies.begin();
	while (it != game.enemies.end()) {
		if ((*it) == this)
			game.enemies.erase(it);
		else
			it++;
	}
}

EnemyBasic::EnemyBasic(EnemyBasic const &src) : AEnemy(src) {
	*this = src;
}

// -- Operators ----------------------------------------------------------------

EnemyBasic &EnemyBasic::operator=(EnemyBasic const &rhs) {
	if ( this != &rhs ) {
		AEnemy::operator=(rhs);
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
bool	EnemyBasic::_update(float const dTime) {
	if (_isBlocked())  // do nothing if blocked
		return true;
	_baseEnemyMove(dTime, _dir);
	return true;
}

/**
 * @brief postUpdate is called each frame. After update()
 *
 * @return true if success
 * @return false if failure
 */
bool	EnemyBasic::_postUpdate() {
	return true;
}

/**
 * @brief draw is called each frame.
 *
 * @return true if success
 * @return false if failure
 */
bool	EnemyBasic::_draw(Gui &gui) {
	gui.drawCube(Block::IA, getPos());
	return true;
}
