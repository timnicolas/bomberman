#include "Enemy.hpp"
#include "Player.hpp"

// -- Constructors -------------------------------------------------------------

Enemy::Enemy(SceneGame &game)
: AEnemy(game),
  _directionsOrder{Direction::UP, Direction::DOWN, Direction::LEFT, Direction::RIGHT},
  _dirIdx(0)
{
	name = "Enemy";
}

Enemy::~Enemy() {
	std::vector<ACharacter *>::iterator it = game.enemies.begin();
	while (it != game.enemies.end()) {
		if ((*it) == this)
			game.enemies.erase(it);
		else
			it++;
	}
}

Enemy::Enemy(Enemy const &src) : AEnemy(src) {
	*this = src;
}

// -- Operators ----------------------------------------------------------------

Enemy &Enemy::operator=(Enemy const &rhs) {
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
bool	Enemy::_update(float const dTime) {
	_movePatternBasic(dTime, _directionsOrder, _dirIdx);
	return true;
}

/**
 * @brief postUpdate is called each frame. After update()
 *
 * @return true if success
 * @return false if failure
 */
bool	Enemy::_postUpdate() {
	return true;
}

/**
 * @brief draw is called each frame.
 *
 * @return true if success
 * @return false if failure
 */
bool	Enemy::_draw(Gui &gui) {
	gui.drawCube(Block::IA, getPos());
	return true;
}

/**
 * @brief Static class to generate enemies
 *
 * @param game
 * @param rate Probability to generate an enemy is between 0 and 1.
 * @return Enemy*
 */
Enemy*	Enemy::generateEnemy(SceneGame &game, float rate) {
	if (rate <= 0.0f)
		return nullptr;
	if (rate >= 1.0f)
		return new Enemy(game);

	int		percentRate = rand() % 100;
	// logInfo("percentRate: " << percentRate);
	// logInfo("rate: " << static_cast<int>(rate * 100));
	if (percentRate > static_cast<int>(rate * 100))
		return nullptr;
	return new Enemy(game);
}
