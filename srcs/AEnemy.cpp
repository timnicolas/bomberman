#include "AEnemy.hpp"
#include "Player.hpp"

// -- Constructors -------------------------------------------------------------

AEnemy::AEnemy(SceneGame &game)
: ACharacter(game)
{
	name = "AEnemy";
	type = Type::ENEMY;
}

AEnemy::~AEnemy() {
	// TODO(ebaudet): correct segfault
	// std::vector<ACharacter *>::iterator find;
	// find = std::find(game->enemies.begin(), game->enemies.end(), this);
	// if (find != game->enemies.end()) {
	// 	game->enemies.erase(find);
	// }
}

AEnemy::AEnemy(AEnemy const &src) : ACharacter(src) {
	*this = src;
}

// -- Operators ----------------------------------------------------------------

AEnemy &AEnemy::operator=(AEnemy const &rhs) {
	if ( this != &rhs ) {
		logWarn("Enemy object copied");
		ACharacter::operator=(rhs);
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
bool	AEnemy::update(float const dTime) {
	if (!active)
		return true;
	if (!alive)
		active = false;
	if (game.player->hasCollision(position)) {
		game.player->takeDamage(1);
	}
	return _update(dTime);
}

/**
 * @brief postUpdate is called each frame. After update()
 *
 * @return true if success
 * @return false if failure
 */
bool	AEnemy::postUpdate() {
	if (!active) {
		delete this;
		return false;
	}
	return _postUpdate();
}

/**
 * @brief draw is called each frame.
 *
 * @return true if success
 * @return false if failure
 */
bool	AEnemy::draw(Gui &gui) {
	return _draw(gui);
}

/**
 * @brief Move and follow a basic pattern (list of direction).
 *
 * start at directionOrder[startDirID] and try to move.
 * If cannot move, try all the next direction until one direction is valid.
 *
 * @param dTime Delta Time
 * @param directionOrder A vector with all direction in the right order
 * @param dirIdx The direction used (index in directionOrder)
 * @return false If the enemy is blocked
 */
bool AEnemy::_movePatternBasic(float const dTime, std::vector<Direction::Enum> directionOrder, uint32_t & dirIdx) {
	if (dirIdx >= directionOrder.size()) {
		logWarn("invalid index for AEnemy::_movePatternBasic(): " << dirIdx << " auto set index to 0");
		dirIdx = 0;
	}
	glm::vec3 pos = getPos();
	for (uint32_t i = dirIdx; i < directionOrder.size(); i++) {
		if (pos != _moveTo(directionOrder[i], dTime)) {
			dirIdx = i;
			return true;
		}
	}
	for (uint32_t i = 0; i < dirIdx; i++) {
		if (pos != _moveTo(directionOrder[i], dTime)) {
			dirIdx = i;
			return true;
		}
	}
	return false;
}

/**
 * @brief If the player is visible by the entity, return the direction of the player
 *
 * @return Direction::Enum Return the direction of the player (NO_DIRECTION if not visible)
 */
Direction::Enum AEnemy::_isPlayerVisible() const {
	glm::ivec2 playerPos = game.player->getIntPos();
	glm::ivec2 thisPos = getIntPos();
	for (int x = thisPos.x; x < static_cast<int>(game.size.x); x++) {
		if (x == playerPos.x)
			return Direction::RIGHT;
		if (game.board[x][thisPos.y].size() > 0)
			break;
	}
	for (int x = thisPos.x; x >= 0; x--) {
		if (x == playerPos.x)
			return Direction::LEFT;
		if (game.board[x][thisPos.y].size() > 0)
			break;
	}
	for (int y = thisPos.y; y < static_cast<int>(game.size.y); y++) {
		if (y == playerPos.y)
			return Direction::DOWN;
		if (game.board[thisPos.x][y].size() > 0)
			break;
	}
	for (int y = thisPos.y; y >= 0; y--) {
		if (y == playerPos.y)
			return Direction::UP;
		if (game.board[thisPos.x][y].size() > 0)
			break;
	}

	return Direction::NO_DIRECTION;
}

// -- Exceptions errors --------------------------------------------------------

AEnemy::EnemyException::EnemyException()
: std::runtime_error("AEnemy Exception") {}

AEnemy::EnemyException::EnemyException(const char* whatArg)
: std::runtime_error(std::string(std::string("EnemyError: ") + whatArg).c_str()) {}
