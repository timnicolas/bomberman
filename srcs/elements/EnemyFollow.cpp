#include "EnemyFollow.hpp"

// -- Constructors -------------------------------------------------------------

EnemyFollow::EnemyFollow(SceneGame &game)
: AEnemy(game),
  _path()
{
	name = "EnemyFollow";

	// TODO(tnicolas42) remove this
	_path.push_back({Direction::RIGHT, {9, 1}});
	_path.push_back({Direction::DOWN, {9, 5}});
	_path.push_back({Direction::LEFT, {8, 5}});
	_path.push_back({Direction::DOWN, {8, 7}});
	_path.push_back({Direction::RIGHT, {11, 7}});
	_path.push_back({Direction::UP, {11, 5}});
}

EnemyFollow::~EnemyFollow() {
	auto it = game.enemies.begin();
	while (it != game.enemies.end()) {
		if ((*it) == this)
			game.enemies.erase(it);
		else
			it++;
	}
}

EnemyFollow::EnemyFollow(EnemyFollow const &src) : AEnemy(src) {
	*this = src;
}

// -- Operators ----------------------------------------------------------------

EnemyFollow &EnemyFollow::operator=(EnemyFollow const &rhs) {
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
bool	EnemyFollow::_update(float const dTime) {
	if (_followPath(dTime, _path) == false) {
		logWarn("invalid path");
	}
	if (_path.size() == 0) {
		logSuccess("arrived to destination");
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
	gui.drawCube(Block::IA, getPos());
	return true;
}

/**
 * @brief Static class to generate enemies
 *
 * @param game
 * @param rate Probability to generate an enemy is between 0 and 1.
 * @return EnemyFollow*
 */
EnemyFollow*	EnemyFollow::generateEnemy(SceneGame &game, float rate) {
	if (rate <= 0.0f)
		return nullptr;
	if (rate >= 1.0f)
		return new EnemyFollow(game);

	int		percentRate = rand() % 100;
	// logInfo("percentRate: " << percentRate);
	// logInfo("rate: " << static_cast<int>(rate * 100));
	if (percentRate > static_cast<int>(rate * 100))
		return nullptr;
	return new EnemyFollow(game);
}
