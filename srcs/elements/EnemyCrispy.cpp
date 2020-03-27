#include "EnemyCrispy.hpp"
#include "Player.hpp"

// -- Constructors -------------------------------------------------------------

EnemyCrispy::EnemyCrispy(SceneGame &game)
: AEnemy(game),
  _isWall(true),
  _dir(Direction::UP),
  _playerDir(Direction::NO_DIRECTION)
{
	name = "EnemyCrispy";
}

EnemyCrispy::~EnemyCrispy() {
	auto it = game.enemies.begin();
	while (it != game.enemies.end()) {
		if ((*it) == this)
			game.enemies.erase(it);
		else
			it++;
	}
}

EnemyCrispy::EnemyCrispy(EnemyCrispy const &src) : AEnemy(src) {
	*this = src;
}

// -- Operators ----------------------------------------------------------------

EnemyCrispy &EnemyCrispy::operator=(EnemyCrispy const &rhs) {
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
bool	EnemyCrispy::_update(float const dTime) {
	if (_isBlocked())  // do nothing if blocked
		return true;

	Direction::Enum viewPlayerDir = _isPlayerVisible();
	if (viewPlayerDir != Direction::NO_DIRECTION) {
		_playerDir = viewPlayerDir;
		_dir = viewPlayerDir;
		_isWall = false;
	}
	if (!_isWall) {
		glm::vec3 tmpPos = position;
		if (tmpPos == _moveTo(_playerDir, dTime)) {
			_playerDir = Direction::NO_DIRECTION;
			_baseEnemyMove(dTime, _dir);
		}
	}
	return true;
}

/**
 * @brief postUpdate is called each frame. After update()
 *
 * @return true if success
 * @return false if failure
 */
bool	EnemyCrispy::_postUpdate() {
	return true;
}

/**
 * @brief draw is called each frame.
 *
 * @return true if success
 * @return false if failure
 */
bool	EnemyCrispy::_draw(Gui &gui) {
	Block::Enum blockType = Block::IA;
	if (_isWall) {
		blockType = Block::DESTRUCTIBLE_WALL;
	}
	gui.drawCube(blockType, getPos());
	return true;
}
