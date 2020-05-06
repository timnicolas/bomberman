#include "EnemyCrispy.hpp"
#include "Player.hpp"
#include "AudioManager.hpp"

// -- Constructors -------------------------------------------------------------

EnemyCrispy::EnemyCrispy(SceneGame &game)
: AEnemy(game),
  _isWall(true),
  _playerDir(Direction::NO_DIRECTION),
  _lastPayerSeenMs(0)
{
	name = ENEMY_CRISPY_STR;
	AudioManager::loadSound(ENEMY_CRISPY_DEATH_SOUND);
	_soundOfDeath = ENEMY_CRISPY_DEATH_SOUND;
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
		_isWall = rhs._isWall;
		_playerDir = rhs._playerDir;
		_lastPayerSeenMs = rhs._lastPayerSeenMs;
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
bool	EnemyCrispy::_update() {
	if (_isBlocked())  // do nothing if blocked
		return true;

	/* retransform to a wall */
	if (getMs().count() - _lastPayerSeenMs > TIME_BEFORE_TRANSFORM_TO_WALL &&
		_isOn(getIntPos(), game.getDtTime() * speed * 3))
	{
		position.x = getIntPos().x;
		position.z = getIntPos().y;
		_isWall = true;
	}

	/* try to find player */
	Direction::Enum viewPlayerDir = _isPlayerVisible();
	if (viewPlayerDir != Direction::NO_DIRECTION) {
		_playerDir = viewPlayerDir;
		_dir = viewPlayerDir;
		_isWall = false;
		_lastPayerSeenMs = getMs().count();
	}

	/* if we cannot see the player, but still close, EnemyCrispy is active */
	if (_isWall) {
		if (_isPlayerClose(1)) {
			_isWall = false;
			_lastPayerSeenMs = getMs().count();
			_playerDir = Direction::NO_DIRECTION;
		}
	}

	/* move */
	if (!_isWall) {
		glm::vec3 tmpPos = position;
		if (tmpPos == _moveTo(_playerDir)) {
			_playerDir = Direction::NO_DIRECTION;
			_baseEnemyMove(_dir);
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
	if (_isWall)
		gui.drawCube(Block::DESTRUCTIBLE_WALL, getPos());
	else
		gui.drawCube(Block::IA, getPos(), size);
	return true;
}

/**
 * @brief Return if the player is close.
 *
 * @param distance to search the player
 * @return true if close
 * @return false otherwise.
 */
bool	EnemyCrispy::_isPlayerClose(int distance) {
	glm::ivec2 playerPos = game.player->getIntPos();
	glm::ivec2 thisPos = getIntPos();

	if ((playerPos.x >= (thisPos.x - distance)) && (playerPos.x <= (thisPos.x + distance))) {
		if ((playerPos.y >= (thisPos.y - distance)) && (playerPos.y <= (thisPos.y + distance))) {
			return true;
		}
	}
	return false;
}
