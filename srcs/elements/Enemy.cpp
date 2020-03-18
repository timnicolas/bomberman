#include "Enemy.hpp"
#include "Player.hpp"

// -- Constructors -------------------------------------------------------------

Enemy::Enemy(SceneGame &game) : ACharacter(game) {
	type = Type::ENEMY;
	name = "Enemy";
	_direction = Dirrection::LEFT;
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

Enemy::Enemy(Enemy const &src) : ACharacter(src) {
	*this = src;
}

// -- Operators ----------------------------------------------------------------

Enemy &Enemy::operator=(Enemy const &rhs) {
	if ( this != &rhs ) {
		ACharacter::operator=(rhs);
		_direction = rhs._direction;
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
bool	Enemy::update(float const dTime) {
	if (!active)
		return true;
	if (!alive)
		active = false;
	glm::vec3 pos = getPos();
	if (pos == _moveTo(_direction, dTime)) {
		_direction = static_cast<Dirrection::Enum>(((_direction + 1) % Dirrection::NB_DIRECTIONS));
	}
	if (game.player->hasCollision(position)) {
		game.player->takeDamage(1);
	}
	return true;
}

/**
 * @brief postUpdate is called each frame. After update()
 *
 * @return true if success
 * @return false if failure
 */
bool	Enemy::postUpdate() {
	if (!active) {
		delete this;
		return false;
	}
	return true;
}

/**
 * @brief draw is called each frame.
 *
 * @return true if success
 * @return false if failure
 */
bool	Enemy::draw(Gui &gui) {
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
	if (percentRate > static_cast<int>(rate * 100))
		return nullptr;
	return new Enemy(game);
}

// -- Exceptions errors --------------------------------------------------------

Enemy::EnemyException::EnemyException()
: std::runtime_error("Enemy Exception") {}

Enemy::EnemyException::EnemyException(const char* whatArg)
: std::runtime_error(std::string(std::string("EnemyError: ") + whatArg).c_str()) {}
