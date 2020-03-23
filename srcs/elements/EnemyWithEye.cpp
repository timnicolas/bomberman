#include "EnemyWithEye.hpp"
#include "Player.hpp"

// -- Constructors -------------------------------------------------------------

EnemyWithEye::EnemyWithEye(SceneGame &game)
: AEnemy(game),
  _playerDir(Direction::NO_DIRECTION),
  _directionsOrder{Direction::LEFT, Direction::DOWN, Direction::UP, Direction::RIGHT},
  _dirIdx(0)
{
	name = "EnemyWithEye";
}

EnemyWithEye::~EnemyWithEye() {
	auto it = game.enemies.begin();
	while (it != game.enemies.end()) {
		if ((*it) == this)
			game.enemies.erase(it);
		else
			it++;
	}
}

EnemyWithEye::EnemyWithEye(EnemyWithEye const &src) : AEnemy(src) {
	*this = src;
}

// -- Operators ----------------------------------------------------------------

EnemyWithEye &EnemyWithEye::operator=(EnemyWithEye const &rhs) {
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
bool	EnemyWithEye::_update(float const dTime) {
	Direction::Enum dir = _isPlayerVisible();
	if (dir != Direction::NO_DIRECTION)
		_playerDir = dir;
	glm::vec3 tmpPos = position;
	if (tmpPos == _moveTo(_playerDir, dTime)) {
		_playerDir = Direction::NO_DIRECTION;
		_movePatternBasic(dTime, _directionsOrder, _dirIdx);
	}
	return true;
}

/**
 * @brief postUpdate is called each frame. After update()
 *
 * @return true if success
 * @return false if failure
 */
bool	EnemyWithEye::_postUpdate() {
	return true;
}

/**
 * @brief draw is called each frame.
 *
 * @return true if success
 * @return false if failure
 */
bool	EnemyWithEye::_draw(Gui &gui) {
	gui.drawCube(Block::IA, getPos());
	return true;
}

/**
 * @brief Static class to generate enemies
 *
 * @param game
 * @param rate Probability to generate an enemy is between 0 and 1.
 * @return EnemyWithEye*
 */
EnemyWithEye*	EnemyWithEye::generateEnemy(SceneGame &game, float rate) {
	if (rate <= 0.0f)
		return nullptr;
	if (rate >= 1.0f)
		return new EnemyWithEye(game);

	int		percentRate = rand() % 100;
	// logInfo("percentRate: " << percentRate);
	// logInfo("rate: " << static_cast<int>(rate * 100));
	if (percentRate > static_cast<int>(rate * 100))
		return nullptr;
	return new EnemyWithEye(game);
}
