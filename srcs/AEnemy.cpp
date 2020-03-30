#include <stack>
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
 * @return true if success
 * @return false if failure
 */
bool	AEnemy::update() {
	if (!active)
		return true;
	if (!alive)
		active = false;
	if (game.player->hasCollision(position)) {
		game.player->takeDamage(1);
	}
	return _update();
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
 * @param directionOrder A vector with all direction in the right order
 * @param dirIdx The direction used (index in directionOrder)
 * @return false If the enemy is blocked
 */
bool AEnemy::_movePatternBasic(std::vector<Direction::Enum> directionOrder, uint32_t & dirIdx) {
	if (dirIdx >= directionOrder.size()) {
		logWarn("invalid index for AEnemy::_movePatternBasic(): " << dirIdx << " auto set index to 0");
		dirIdx = 0;
	}
	glm::vec3 pos = getPos();
	for (uint32_t i = dirIdx; i < directionOrder.size(); i++) {
		if (pos != _moveTo(directionOrder[i])) {
			dirIdx = i;
			return true;
		}
	}
	for (uint32_t i = 0; i < dirIdx; i++) {
		if (pos != _moveTo(directionOrder[i])) {
			dirIdx = i;
			return true;
		}
	}
	return false;
}

/**
 * @brief Follow the given path
 *
 * @param path The path to follow
 * @return false If the Entity is blocked
 */
bool AEnemy::_followPath(std::deque<PathNode> & path) {
	// if the path is finished
	if (path.size() == 0)
		return true;

	while (1) {
		// if the current path node is finished
		if (_isOn(path.front().goal)) {
			path.pop_front();
		}
		else {
			break;
		}
		// if the path is finished
		if (path.size() == 0)
			return true;
	}

	// if the path is finished
	if (path.size() == 0)
		return true;

	// move
	glm::vec3 pos = getPos();
	if (pos == _moveTo(path.front().dir))
		return false;

	return true;
}

/**
 * @brief If the player is visible by the entity, return the direction of the player
 *
 * @return Direction::Enum Return the direction of the player (NO_DIRECTION if not visible)
 */
Direction::Enum AEnemy::_isPlayerVisible() const {
	glm::ivec2 playerPos = game.player->getIntPos();
	glm::ivec2 thisPos = getIntPos();
	if (playerPos.y == thisPos.y) {
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
	}
	else if (playerPos.x == thisPos.x) {
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
	}

	return Direction::NO_DIRECTION;
}

/**
 * @brief Check if the enemy is on dest
 *
 * @param dest The postion to compare with enemy pos
 * @return true If is on the destination
 */
bool AEnemy::_isOn(glm::ivec2 dest) const {
	if (position.x >= static_cast<float>(dest.x) - IS_ON_POS_OFFSET
	&& position.x <= static_cast<float>(dest.x) + IS_ON_POS_OFFSET
	&& position.z >= static_cast<float>(dest.y) - IS_ON_POS_OFFSET
	&& position.z <= static_cast<float>(dest.y) + IS_ON_POS_OFFSET)
		return true;
	return false;
}

// -- Exceptions errors --------------------------------------------------------

AEnemy::EnemyException::EnemyException()
: std::runtime_error("AEnemy Exception") {}

AEnemy::EnemyException::EnemyException(const char* whatArg)
: std::runtime_error(std::string(std::string("EnemyError: ") + whatArg).c_str()) {}
