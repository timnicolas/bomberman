#include <stack>
#include "AEnemy.hpp"
#include "Player.hpp"

// -- Constructors -------------------------------------------------------------

AEnemy::AEnemy(SceneGame &game)
: ACharacter(game),
  strength(1)
{
	name = "AEnemy";
	type = Type::ENEMY;
	points = 200;
}

AEnemy::~AEnemy() {
	std::vector<AEnemy *>::iterator find;
	find = std::find(game.enemies.begin(), game.enemies.end(), this);
	if (find != game.enemies.end()) {
		game.enemies.erase(find);
	}
	if (game.state == GameState::PLAY)
		game.score += points;
}

AEnemy::AEnemy(AEnemy const &src) : ACharacter(src) {
	*this = src;
}

// -- Operators ----------------------------------------------------------------

AEnemy &AEnemy::operator=(AEnemy const &rhs) {
	if ( this != &rhs ) {
		logWarn("Enemy object copied");
		ACharacter::operator=(rhs);
		bombs = rhs.bombs;
		points = rhs.points;
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
	if (strength != 0 && game.player->hasCollision(position)) {
		game.player->takeDamage(strength);
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
 * @brief Base moving function for enemy
 *
 * @param dTime Delta time
 * @param dir The actual direction of the enemy
 * @return false If the enemy is blocked
 */
bool AEnemy::_baseEnemyMove(float const dTime, Direction::Enum & dir) {
	// try to move forward
	glm::vec3 startPos = position;
	if (startPos != _moveTo(dir, dTime, -1)) {
		return true;
	}

	// foreach direction, define the next direction to turn left or right
	Direction::Enum nextDir[4][3] = {
		{Direction::LEFT, Direction::RIGHT, Direction::DOWN},  // after UP, turn LEFT or RIGHT
		{Direction::UP, Direction::DOWN, Direction::LEFT},  // after RIGHT, turn UP or DOWN
		{Direction::LEFT, Direction::RIGHT, Direction::UP},  // after DOWN, turn LEFT or RIGHT
		{Direction::UP, Direction::DOWN, Direction::RIGHT},  // after LEFT, turn UP or DOWN
	};

	bool rightFirst = rand() & 1;  // random the first element btw right and left
	// order to try direction (front, left/right, back)
	Direction::Enum tryDirOrder[3] = {
		nextDir[dir][rightFirst ? 1 : 0],  // then right or left
		nextDir[dir][rightFirst ? 0 : 1],  // then left or right
		nextDir[dir][2],  // then, go back
	};

	glm::ivec2 ipos = getIntPos();
	glm::ivec2 nextPos[4] = {
		{ 0, -1},  // UP
		{ 1,  0},  // RIGHT
		{ 0,  1},  // DOWN
		{-1,  0},  // LEFT
	};

	for (int i = 0; i < 3; i++) {
		glm::ivec2 tmpPos(ipos.x + nextPos[tryDirOrder[i]].x, ipos.y + nextPos[tryDirOrder[i]].y);
		bool canMove = true;
		if (game.positionInGame(tmpPos) == false) {
			canMove = false;
			continue;
		}
		for (auto &&entity : getBoard()[tmpPos.x][tmpPos.y]) {
			if (entity->crossable == Type::ALL || entity->crossable == type)
				continue;
			canMove = false;
		}
		if (canMove) {
			glm::vec3 startPos = position;
			if (startPos != _moveTo(tryDirOrder[i], dTime)) {
				dir = tryDirOrder[i];
				break;
			}
			if (i == 3)
				return false;  // cannot move
		}
	}

	return true;
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
 * @brief Follow the given path
 *
 * @param dTime Delta Time
 * @param path The path to follow
 * @return false If the Entity is blocked
 */
bool AEnemy::_followPath(float const dTime, std::deque<PathNode> & path) {
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
	if (pos == _moveTo(path.front().dir, dTime))
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
			if (getBoard()[x][thisPos.y].size() > 0)
				break;
		}
		for (int x = thisPos.x; x >= 0; x--) {
			if (x == playerPos.x)
				return Direction::LEFT;
			if (getBoard()[x][thisPos.y].size() > 0)
				break;
		}
	}
	else if (playerPos.x == thisPos.x) {
		for (int y = thisPos.y; y < static_cast<int>(game.size.y); y++) {
			if (y == playerPos.y)
				return Direction::DOWN;
			if (getBoard()[thisPos.x][y].size() > 0)
				break;
		}
		for (int y = thisPos.y; y >= 0; y--) {
			if (y == playerPos.y)
				return Direction::UP;
			if (getBoard()[thisPos.x][y].size() > 0)
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
bool AEnemy::_isOn(glm::ivec2 dest, float offset) const {
	if (position.x >= static_cast<float>(dest.x) - offset
	&& position.x <= static_cast<float>(dest.x) + offset
	&& position.z >= static_cast<float>(dest.y) - offset
	&& position.z <= static_cast<float>(dest.y) + offset)
		return true;
	return false;
}

/**
 * @brief Check if the enemy is blocked btw walls
 *
 * @return true If the enemy is blocked
 */
bool AEnemy::_isBlocked() const {
	glm::ivec2 ipos = getIntPos();

	int nbColisions = 0;  // nb of walls around enemy
	// all blocks around enemy
	int nexts[4][2] = {
		{-1,  0},
		{ 1,  0},
		{ 0, -1},
		{ 0,  1},
	};
	for (int i = 0; i < 4; i++) {
		glm::ivec2 tmpPos(ipos.x + nexts[i][0], ipos.y + nexts[i][1]);
		if (game.positionInGame(tmpPos) == false)
			continue;
		for (auto &&entity : getBoard()[tmpPos.x][tmpPos.y]) {
			// don't go into fire if a bomb remove a wall and the enemy was blocked before
			if (entity->type == Type::FIRE) {
				nbColisions++;
				break;
			}
			if (entity->crossable == Type::ALL || entity->crossable == type)
				continue;

			nbColisions++;
			break;
		}
	}
	// il walls all around entity
	if (nbColisions == 4)
		return true;
	return false;
}

// -- Exceptions errors --------------------------------------------------------

AEnemy::EnemyException::EnemyException()
: std::runtime_error("AEnemy Exception") {}

AEnemy::EnemyException::EnemyException(const char* whatArg)
: std::runtime_error(std::string(std::string("EnemyError: ") + whatArg).c_str()) {}
