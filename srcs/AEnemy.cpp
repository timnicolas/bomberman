#include <stack>
#include "AEnemy.hpp"
#include "Player.hpp"
#include "AudioManager.hpp"
#include "EnemyFly.hpp"

// -- Constructors -------------------------------------------------------------

/**
 * @brief Construct a new AEnemy::AEnemy object
 *
 * @param game A reference to the main SceneGame object
 */
AEnemy::AEnemy(SceneGame &game)
: ACharacter(game),
  _dir(Direction::UP),
  strength(1)
{
	name = "AEnemy";
	type = Type::ENEMY;
	points = 200;
	_fisrtCall = true;
	_moveOnCenter = true;
	AudioManager::loadSound(ENEMY_HIT_1_SOUND);
	AudioManager::loadSound(ENEMY_HIT_2_SOUND);
	_soundAttack = {ENEMY_HIT_1_SOUND, ENEMY_HIT_2_SOUND};
}

/**
 * @brief Destroy the AEnemy::AEnemy object
 */
AEnemy::~AEnemy() {
	std::vector<AEnemy *>::iterator find;
	find = std::find(game.enemies.begin(), game.enemies.end(), this);
	if (find != game.enemies.end()) {
		game.enemies.erase(find);
	}
}

/**
 * @brief Construct a new AEnemy::AEnemy object
 *
 * @param src The object to do the copy
 */
AEnemy::AEnemy(AEnemy const &src) : ACharacter(src) {
	*this = src;
}

// -- Operators ----------------------------------------------------------------

/**
 * @brief Copy this object
 *
 * @param rhs The object to copy
 * @return AEnemy& A reference to the copied object
 */
AEnemy &AEnemy::operator=(AEnemy const &rhs) {
	if ( this != &rhs ) {
		logWarn("Enemy object copied");
		ACharacter::operator=(rhs);
		bombs = rhs.bombs;
		points = rhs.points;
		_dir = rhs._dir;
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

	// attack logic
	glm::vec3 tmpPos = position;
	tmpPos.x += (movingSize.x - size.x) / 2;
	tmpPos.z += (movingSize.z - size.z) / 2;
	if (alive && strength != 0 && game.player->active && game.player->alive &&
		game.player->invulnerable <= 0.0f &&
		game.player->hasCollision(tmpPos, size) &&
		_entityState.state != EntityState::ATTACK)
	{
		if (_soundAttack.size()) {
			try {
				std::string soundAttack = _soundAttack[rand() % _soundAttack.size()];
				AudioManager::playSound(soundAttack, 0.6f);
			} catch(Sound::SoundException const & e) {
				logErr(e.what());
			}
		}
		setState(EntityState::ATTACK);
		game.player->takeDamage(strength);

		// facing player on attack (except for EnemyFly)
		if (name != ENEMY_FLY_STR) {
			front = glm::normalize(game.player->position - position);
		}
	}

	bool ret = _update();
	_fisrtCall = false;
	return ret;
}

/**
 * @brief postUpdate is called each frame. After update()
 *
 * @return true if success
 * @return false if failure
 */
bool	AEnemy::postUpdate() {
	if (!active && (_animDeathEnd || !_model)) {
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
 * @brief called on animation end if passed to Model
 *
 * @param animName the current animation name
 */
void	AEnemy::animEndCb(std::string animName) {
	if (animName == "Armature|death") {
		_animDeathEnd = true;
	}
	else if (animName == "Armature|attack") {
		setState(EntityState::IDLE);
	}
}

/**
 * @brief AEnemy Take < damage > damages.
 *
 * @param damage
 * @return true if damage taken
 * @return false if damage not taken
 */
bool	AEnemy::takeDamage(const int damage) {
	bool	wasAlive = alive;
	bool	result = ACharacter::takeDamage(damage);
	if (result) {
		if (wasAlive && !alive) {
			game.enemiesKilled += 1;
			game.score += points;
			game.player->activeBonus.score = 3.0f;
		}
	}

	return result;
}

/**
 * @brief get a list of entity in collision with the Character at a position.
 *
 * @param dest default VOID_POS3
 * @return std::unordered_set<AEntity *> collisions
 */
std::unordered_set<AEntity *>	AEnemy::getCollision(glm::vec3 dest) const {
	std::unordered_set<AEntity *> collisions = ACharacter::getCollision(dest);
	if (name == ENEMY_FLY_STR)
		return collisions;

	/* get all positions blocks under character on a position */
	std::vector<glm::ivec2> allPos = _getAllPositions(dest, size);

	std::vector<glm::ivec2> allPosEnemy;
	for (auto enemy : game.enemies) {
		if (enemy == this)
			continue;
		if (enemy->name == ENEMY_FLY_STR)
			continue;
		allPosEnemy.clear();
		allPosEnemy = _getAllPositions(enemy->getPos(), enemy->size );
		bool found = false;
		for (auto && blockPos : allPos) {
			for (auto && blockPosEnemy : allPosEnemy) {
				if (blockPos == blockPosEnemy) {
					collisions.insert(enemy);
					found = true;
				}
				if (found)
					break;
			}
			if (found)
				break;
		}
	}
	return collisions;
}

// -- Protected methods --------------------------------------------------------

/**
 * @brief Base moving function for enemy
 *
 * @param dir The actual direction of the enemy
 * @return false If the enemy is blocked
 */
bool AEnemy::_baseEnemyMove(Direction::Enum & dir) {
	// try to move forward
	glm::vec3 startPos = position;
	if (startPos != _moveTo(dir)) {
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
		if (_canWalkOnBlock(tmpPos)) {
			glm::vec3 startPos = position;
			if (startPos != _moveTo(tryDirOrder[i])) {
				dir = tryDirOrder[i];
				break;
			}
			if (i == 3) {
				return false;  // cannot move
			}
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
		if (path.size() == 0) {
			return true;
		}
	}

	// if the path is finished
	if (path.size() == 0) {
		return true;
	}

	// move
	glm::vec3 pos = getPos();
	if (pos == _moveTo(path.front().dir)) {
		return false;
	}

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

	// return last direction when they are on the same square
	if (playerPos == thisPos) {
		return _dir;
	}

	// can't see a player inside a wall (with the wallpass for example)
	if (!_canMoveOn(glm::vec3(playerPos.x, 0, playerPos.y))) {
		return Direction::NO_DIRECTION;
	}

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
 * @param offset Offset
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
bool AEnemy::_isBlocked() {
	glm::ivec2 ipos = getIntPos();

	int nbCollisions = 0;  // nb of walls around enemy
	// all blocks around enemy
	int nearby[4][2] = {
		{-1,  0},
		{ 1,  0},
		{ 0, -1},
		{ 0,  1},
	};

	for (int i = 0; i < 4; i++) {
		int startVal = nbCollisions;
		glm::ivec2 tmpPos(ipos.x + nearby[i][0], ipos.y + nearby[i][1]);
		if (game.positionInGame(glm::vec3(tmpPos.x, position.y, tmpPos.y), size) == false) {
			continue;
		}
		/* check with static entities */
		for (auto && entity : getBoard()[tmpPos.x][tmpPos.y]) {
			if (entity->type == Type::FIRE || _canWalkOnEntity(entity) == false) {
				nbCollisions++;
				break;
			}
		}
		if (nbCollisions > startVal) {
			continue;
		}
		/* check with enemies */
		for (auto && entity : game.enemies) {
			if (entity != this && entity->getIntPos() == tmpPos && position.y <= 0.2) {
				nbCollisions++;
				break;
			}
		}
	}

	// block on colision or on player attack
	if (nbCollisions == 4)
		return true;

	return false;
}

// -- Exceptions errors --------------------------------------------------------

/**
 * @brief Construct a new AEnemy::EnemyException::EnemyException object
 */
AEnemy::EnemyException::EnemyException()
: std::runtime_error("AEnemy Exception") {}

/**
 * @brief Construct a new AEnemy::EnemyException::EnemyException object
 *
 * @param whatArg Error message
 */
AEnemy::EnemyException::EnemyException(const char* whatArg)
: std::runtime_error(std::string(std::string("EnemyError: ") + whatArg).c_str()) {}
