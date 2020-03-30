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
 * @return true if success
 * @return false if failure
 */
bool	EnemyBasic::_update() {
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
