#include "Enemy.hpp"

// -- Constructors -------------------------------------------------------------

Enemy::Enemy(SceneGame &game) : ACharacter(game) {
	type = Type::ENEMY;
}

Enemy::~Enemy() {
}

Enemy::Enemy(Enemy const &src) : ACharacter(src) {
	*this = src;
}

// -- Operators ----------------------------------------------------------------

Enemy &Enemy::operator=(Enemy const &rhs) {
	if ( this != &rhs ) {
		ACharacter::operator=(rhs);
	}
	return *this;
}

// -- Methods ------------------------------------------------------------------

/**
 * update is called each frame.
 * @param d_time Delta Time
 */
bool	Enemy::update(std::chrono::milliseconds d_time) {
	std::cout << "Last Enemy updated at " << d_time.count() << std::endl;
	return true;
}

/**
 * draw is called each frame.
 */
bool	Enemy::draw() {
	return true;
}

bool	Enemy::isDestructable() {
	return true;
}
bool	Enemy::blockPropagation() {
	return false;
}
bool	Enemy::isAlive() {
	return life;
}

/**
 * Static class to generate enemies
 * @param rate Probability to generate an enemy is between 0 and 1.
 */
Enemy*	Enemy::generateEnemy(SceneGame &game, float rate) {
	srand(time(NULL));

	if (rate <= 0)
		return nullptr;
	if (rate >= 1)
		return new Enemy(game);

	int		percent_rate = rand() % 100;
	if (percent_rate > static_cast<int>(rate * 100))
		return nullptr;
	return new Enemy(game);
}

// -- Exceptions errors --------------------------------------------------------

Enemy::EnemyException::EnemyException()
: std::runtime_error("Enemy Exception") {}

Enemy::EnemyException::EnemyException(const char* what_arg)
: std::runtime_error(std::string(std::string("EnemyError: ") + what_arg).c_str()) {}
