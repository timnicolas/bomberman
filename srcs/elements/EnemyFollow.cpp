#include "EnemyFollow.hpp"
#include "Player.hpp"

// -- Constructors -------------------------------------------------------------

EnemyFollow::EnemyFollow(SceneGame &game)
: AEnemy(game),
  _findPlayer(false),
  _path()
{
	name = "EnemyFollow";
	size = glm::vec3(0.8, 1.5, 0.8);
	_lastFindMs = getMs();
}

EnemyFollow::~EnemyFollow() {
}

EnemyFollow::EnemyFollow(EnemyFollow const &src) : AEnemy(src) {
	*this = src;
}

// -- Operators ----------------------------------------------------------------

EnemyFollow &EnemyFollow::operator=(EnemyFollow const &rhs) {
	if ( this != &rhs ) {
		_findPlayer = rhs._findPlayer;
		_path = rhs._path;
		_lastFindMs = rhs._lastFindMs;
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
bool	EnemyFollow::_update() {
	if (_isBlocked())  // do nothing if blocked
		return true;
	// try to find a path to the player
	// after 1sec, 1 chance over 10 to relaunch path calculation
	if ((getMs() - _lastFindMs).count() > 1000 && (!_findPlayer || rand() % 100 < 10)) {
		_lastFindMs = getMs();
		_findPlayer = _getPathTo(game.player->getIntPos(), _path);
	}
	glm::vec3 lastPos = position;
	if (_path.size() == 0) {
		// arrived to destination
		_findPlayer = false;
	}
	else if (_followPath(_path) == false) {
		// blocked by a wall
		_findPlayer = false;
		_path.clear();
	}

	if (lastPos == position) {
		// if the enemy doesn't move
		_baseEnemyMove(_dir);
	}

	return true;
}

/**
 * @brief postUpdate is called each frame. After update()
 *
 * @return true if success
 * @return false if failure
 */
bool	EnemyFollow::_postUpdate() {
	return true;
}

/**
 * @brief draw is called each frame.
 *
 * @return true if success
 * @return false if failure
 */
bool	EnemyFollow::_draw(Gui &gui) {
	gui.drawCube(Block::IA, getPos(), size);
	return true;
}
