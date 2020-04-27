#include "ACharacter.hpp"
#include "SceneGame.hpp"
#include "useGlm.hpp"
#include "AEnemy.hpp"
#include "AudioManager.hpp"
#include "BoxCollider.hpp"

// -- Constructors -------------------------------------------------------------

ACharacter::ACharacter(SceneGame &game)
: AEntity(game),
  front(0, 0, 1)
{
	category = Category::MOBILE;
	lives = 1;
	speed = 1.0;
	position = {0.0, 0.0, 0.0};
	size = {0.95, 1.5, 0.95};
	movingSize = {0.95, 0.95, 0.95};
	blockPropagation = false;
	destructible = true;
	resetCrossable();
}

ACharacter::~ACharacter() {
}

ACharacter::ACharacter(ACharacter const &src) : AEntity(src) {
	*this = src;
}

// -- Operators ----------------------------------------------------------------

ACharacter &ACharacter::operator=(ACharacter const &rhs) {
	if ( this != &rhs ) {
		AEntity::operator=(rhs);
		lives = rhs.lives;
		speed = rhs.speed;
	}
	return *this;
}

// -- Methods ------------------------------------------------------------------

/**
 * @brief Draw the collider of the entity
 *
 * @return false If failed
 */
bool		ACharacter::drawCollider() {
	glm::vec4 color = colorise(s.j("colors").j("collider").u("color"), s.j("colors").j("collider").u("alpha"));
	glm::vec3 tmpPos = position;
	tmpPos.x += (movingSize.x - size.x) / 2;
	tmpPos.z += (movingSize.z - size.z) / 2;
	BoxCollider::drawBox(tmpPos, size, color);
	return true;
}

/**
 * @brief Set the Entity that the Character can cross
 */
void ACharacter::resetCrossable() {
	crossableTypes.clear();
	crossableTypes.push_back(Type::FIRE);
	crossableTypes.push_back(Type::BONUS);
}

/**
 * @brief Get the current postion of the ACharacter
 *
 * @return glm::vec2
 */
glm::vec3		ACharacter::getPos() const {
	return position;
}

/**
 * @brief Get the current position (in integer)
 *
 * @return glm::ivec2 The integer position
 */
glm::ivec2		ACharacter::getIntPos() const {
	return glm::ivec2(
		static_cast<int>(position.x + size.x / 2),
		static_cast<int>(position.z + size.z / 2)
	);
}

/**
 * @brief Init the Class. Needed to be called before any usage of the Class.
 *
 * @param pos
 * @return ACharacter*
 */
ACharacter		*ACharacter::setPosition(glm::vec3 pos) {
	this->position = pos;
	if (_model != nullptr) {
		_model->transform.setPos(position + glm::vec3(movingSize.x / 2, 0, movingSize.z / 2));
	}
	return this;
}

/**
 * @brief If the character still have lives, he is alive.
 *
 * @return true if alive
 * @return false if dead
 */
bool	ACharacter::isAlive() {
	return lives;
}

/**
 * @brief Get the board (game.board or game.boardFly)
 *
 * @return std::vector< std::vector< std::vector<AEntity *> > >& A reference to the board
 */
std::vector< std::vector< std::vector<AEntity *> > > const & ACharacter::getBoard() const {
	if (position.y >= 0.5)
		return game.boardFly;
	return game.board;
}
/**
 * @brief Get the board (game.board or game.boardFly)
 *
 * @return std::vector< std::vector< std::vector<AEntity *> > >& A reference to the board
 */
std::vector< std::vector< std::vector<AEntity *> > > & ACharacter::getBoard() {
	if (position.y >= 0.5)
		return game.boardFly;
	return game.board;
}

/**
 * @brief Character Take <damage> damages.
 *
 * @param damage
 * @return true if damage taken
 * @return false if damage not taken
 */
bool	ACharacter::takeDamage(const int damage) {
	bool wasAlive = alive;

	if (!active || !destructible)
		return false;
	lives -= damage;
	if (lives <= 0) {
		lives = 0;
		alive = false;
	}

	if (wasAlive && !alive) {
		if (_soundOfDeath.size() > 0) {
			try {
				AudioManager::playSound(_soundOfDeath);
			} catch(Sound::SoundException const & e) {
				logErr(e.what());
			}
		}
		setState(EntityState::DYING);
	}

	return true;
}

/**
 * @brief get a list of entity in collision with the Character at a position.
 *
 * @param pos default VOID_POS3
 * @return std::unordered_set<AEntity *> collisions
 */
std::unordered_set<AEntity *>	ACharacter::getCollision(glm::vec3 dest) const {
	if (dest == VOID_POS3) {
		logWarn("dest is VOID_POS3 in ACharacter::getCollision");
		dest = getPos();
	}
	std::unordered_set<AEntity *> collisions;

	/* get all positions blocks under character on a position */
	std::vector<glm::ivec2> allPos = _getAllPositions(dest, movingSize);

	// std::cout << glm::to_string(dest) << " | ";
	for (auto && blockPos : allPos) {
		// std::cout << glm::to_string(blockPos) << " ";
		if (game.positionInGame(glm::vec3(blockPos.x, position.y, blockPos.y), movingSize) == false) {
			continue;
		}
		for (auto &&entity : getBoard()[blockPos.x][blockPos.y]) {
			collisions.insert(entity);
		}
	}
	// std::cout << std::endl;
	return collisions;
}

/**
 * @brief Test if Character have a collision at position <atPosition>.
 *
 * @param atPosition
 * @param atSize
 * @return true if has a collision
 * @return false if no collision
 */
bool	ACharacter::hasCollision(glm::vec3 atPosition, glm::vec3 atSize) {
	// check if there is no colision (easier)
	if ((position.x >= atPosition.x + atSize.x) ||  // too much right
		(position.x + movingSize.x <= atPosition.x) ||  // too much left
		(position.z >= atPosition.z + atSize.z) ||  // too much down
		(position.z + movingSize.z <= atPosition.z)) {  // too much up
		return false;
	}

	return true;
}

/**
 * @brief Teleport a character on a position (tp check colision)
 *
 * @param tpPos The position to teleport
 * @return true If the tp is a success
 */
bool	ACharacter::tp(glm::vec3 tpPos) {
	if (_canMoveOn(tpPos)) {
		position = tpPos;
		return true;
	}
	return false;
}

// -- Protected Methods --------------------------------------------------------

/**
 * @brief Get all entities that block the player on a given position
 *
 * @return std::unordered_set<AEntity *> The list of entity
 */
std::unordered_set<AEntity *> ACharacter::_getAllBlockableEntity(glm::vec3 dest) const {
	// all entity on character
	std::unordered_set<AEntity *> allCollisionsEntity = getCollision(dest);
	// all entity that block character
	std::unordered_set<AEntity *> allCollisionsBlock;

	for (auto && entity : allCollisionsEntity) {
		if (!_canWalkOnEntity(entity))
			allCollisionsBlock.insert(entity);
	}

	return allCollisionsBlock;
}

/**
 * @brief Check if we can walk on a block
 *
 * @param pos The block pos
 * @return true If we can walk on this block
 */
bool ACharacter::_canWalkOnBlock(glm::ivec2 pos) const {
	/* check if we are on the game board */
	if (game.positionInGame(glm::vec3(pos.x, position.y, pos.y), movingSize) == false) {
		return false;
	}

	for (auto && entity : getBoard()[pos.x][pos.y]) {
		if (_canWalkOnEntity(entity) == false)
			return false;
	}
	return true;
}

/**
 * @brief Check if we can walk on an entity
 *
 * @param pos The block pos
 * @return true If we can walk on this block
 */
bool ACharacter::_canWalkOnEntity(AEntity * entity) const {
	bool ok = false;
	for (auto && crossable : crossableTypes) {
		if (entity->type == Type::ALL || crossable == Type::ALL || entity->type == crossable) {
			ok = true;
		}
	}
	return ok;
}

/**
 * @brief Check if the entity can move on a position
 *
 * @param dest The position to check
 * @return true If the entity can move
 */
bool	ACharacter::_canMoveOn(glm::vec3 dest) const {
	/* check if we are on the game board */
	if (game.positionInGame(dest, movingSize) == false) {
		return false;
	}

	/* check colision with all entities under character */
	std::unordered_set<AEntity *> allCollisionsEntity = getCollision(dest);
	for (auto && entity : allCollisionsEntity) {
		if (!_canWalkOnEntity(entity))
			return false;
	}
	return true;
}

/**
 * @brief Check if the entity can move on from a position to a destination
 *
 * This function allow you to put bomb and walk on it
 *
 * @param from The actual position
 * @param to The goal position
 * @return true If the entity can move
 */
bool	ACharacter::_canMoveOnFromTo(glm::vec3 from, glm::vec3 to) const {
	/* check if we are on the game board */
	if (game.positionInGame(to, movingSize) == false) {
		return false;
	}

	/* check colision with all entities under character */
	std::unordered_set<AEntity *> allCollisionsEntityNow = _getAllBlockableEntity(to);
	if (allCollisionsEntityNow.empty())
		return true;
	// if there is a collision, check if there was the same collision in last time
	std::unordered_set<AEntity *> allCollisionsEntityLast = _getAllBlockableEntity(from);
	if (allCollisionsEntityNow.empty())
		return false;
	for (auto && nowEnt : allCollisionsEntityNow) {
		bool ok = false;
		for (auto && lastEnt : allCollisionsEntityLast) {
			if (nowEnt == lastEnt) {
				ok = true;
				break;
			}
		}
		if (!ok)
			return false;
	}
	return true;
}

/**
 * @brief Move to direction if possible.
 *
 * @param direction Direction to move
 * @param offset Offset to turn correction (-1 to don't use correction)
 * @return glm::vec3 finale position
 */
glm::vec3	ACharacter::_moveTo(Direction::Enum direction, float const offset) {
	glm::vec3 	dir = glm::vec3(0, front.y, 0);

	switch (direction) {
		case Direction::UP:
			dir.z = -1;
			break;
		case Direction::RIGHT:
			dir.x = 1;
			break;
		case Direction::DOWN:
			dir.z = 1;
			break;
		case Direction::LEFT:
			dir.x = -1;
			break;
		default:
			return position;
	}

	_moveTo(dir, offset);

	return position;
}

/**
 * @brief get all positions blocks under character on a position
 *
 * @param pos
 * @param sz
 * @return std::vector<glm::ivec2>
 */
std::vector<glm::ivec2>	ACharacter::_getAllPositions(glm::vec3 pos, glm::vec3 sz) const {
	std::vector<glm::ivec2> allPos;
	glm::ivec2 posInt = glm::ivec2(static_cast<int>(pos.x), static_cast<int>(pos.z));
	glm::ivec2 tmpPos;

	allPos.push_back(posInt);

	if (static_cast<int>(pos.x + sz.x) > posInt.x) {
		tmpPos = posInt;
		tmpPos.x += 1;
		allPos.push_back(tmpPos);

		if (static_cast<int>(pos.z + sz.z) > posInt.y) {
			tmpPos = posInt;
			tmpPos.x += 1;
			tmpPos.y += 1;
			allPos.push_back(tmpPos);
		}
	}
	if (static_cast<int>(pos.z + sz.z) > posInt.y) {
		tmpPos = posInt;
		tmpPos.y += 1;
		allPos.push_back(tmpPos);
	}
	return allPos;
}

/**
 * @brief Move to direction if possible.
 *
 * @param direction Direction to move
 * @param offset Offset to turn correction (-1 to don't use correction)
 * @return glm::vec3 finale position
 */
glm::vec3	ACharacter::_moveTo(glm::vec3 direction, float const offset) {
	glm::vec3 beforePosition = position;
	direction = glm::normalize(direction);

	// update state on first move
	if (_entityState.state != EntityState::RUNNING) {
		setState(EntityState::RUNNING);
	}

	if (glm::length(direction) == 0)
		return position;

	glm::vec3 movement = direction * speed * game.getDtTime();

	/* moving help */
	bool reloadMovement = false;
	if (offset > 0 && (direction.x == 0 || direction.z == 0) && !_canMoveOnFromTo(position, position + movement)) {
		// move only one one direction (UP | DOWN | LEFT | RIGHT & movement impossible)
		if (direction.x == 0) {  // up | down
			glm::vec3 tmpPos = position + movement;
			tmpPos.x = static_cast<int>(position.x + movingSize.x - 1);
			if ((position.x + movingSize.x - 1) - tmpPos.x < offset && _canMoveOnFromTo(position, tmpPos)) {
				reloadMovement = true;
				direction.x = -1;
				direction.z = 0;
			}
			tmpPos = position + movement;
			tmpPos.x = static_cast<int>(position.x + 1);
			if (position.x - tmpPos.x > -offset && _canMoveOnFromTo(position, tmpPos)) {
				reloadMovement = true;
				direction.x = 1;
				direction.z = 0;
			}
		}
		else {  // left | right
			glm::vec3 tmpPos = position + movement;
			tmpPos.z = static_cast<int>(position.z + movingSize.z - 1);
			if ((position.z + movingSize.z - 1) - tmpPos.z < offset && _canMoveOnFromTo(position, tmpPos)) {
				reloadMovement = true;
				direction.x = 0;
				direction.z = -1;
			}
			tmpPos = position + movement;
			tmpPos.z = static_cast<int>(position.z + 1);
			if (position.z - tmpPos.z > -offset && _canMoveOnFromTo(position, tmpPos)) {
				reloadMovement = true;
				direction.x = 0;
				direction.z = 1;
			}
		}
	}

	/* diagonal on a wall */
	/*
	*    |
	*   p|
	*    |
	*
	* if right & up -> only UP (because there is a wall on the right)
	*/
	if (direction.x != 0 && direction.z != 0) {
		// try x moving
		glm::vec3 tmpPos = position;
		tmpPos.x += MOVE_STEP * ((direction.x < 0) ? -1 : 1);
		if (!_canMoveOnFromTo(position, tmpPos)) {  // if can't move on x Direction
			reloadMovement = true;
			direction.x = 0;
		}
		// try z moving
		tmpPos = position;
		tmpPos.z += MOVE_STEP * ((direction.z < 0) ? -1 : 1);
		if (!_canMoveOnFromTo(position, tmpPos)) {  // if can't move on x Direction
			reloadMovement = true;
			direction.z = 0;
		}
	}

	/* reload direction vector */
	if (reloadMovement) {  // direction vector has changed so reload final movement
		direction = glm::normalize(direction);
		if (glm::length(direction) == 0)
			return position;
		movement = direction * speed * game.getDtTime();
	}

	/* move step by step */
	float xDir = (movement.x < 0) ? -1 : ((movement.x > 0) ? 1 : 0);
	float zDir = (movement.z < 0) ? -1 : ((movement.z > 0) ? 1 : 0);
	float xAdd = MOVE_STEP * xDir;
	float zAdd = MOVE_STEP * zDir;
	float x = xAdd;
	float z = zAdd;
	bool xEnd = (xDir == 0) ? true : false;
	bool zEnd = (zDir == 0) ? true : false;
	while (!xEnd || !zEnd || x != 0 || z != 0) {
		if (xEnd && x != 0) {
			_miniMove({x, 0, 0});
			x = 0;
		}
		if (!xEnd) {
			if ((xDir > 0 && x > movement.x) || (xDir < 0 && x < movement.x)) {
				xEnd = true;
				x = movement.x - (x - xAdd);
			}
			if (!xEnd) {
				glm::vec3 lastPos = position;
				if (lastPos == _miniMove({xAdd, 0, 0})) {
					xEnd = true;
					x = 0;
				}
			}
			if (!xEnd)
				x += xAdd;
		}

		if (zEnd && z != 0) {
			_miniMove({0, 0, z});
			z = 0;
		}
		if (!zEnd) {
			if ((zDir > 0 && z > movement.z) || (zDir < 0 && z < movement.z)) {
				zEnd = true;
				z = movement.z - (z - zAdd);
			}
			if (!zEnd) {
				glm::vec3 lastPos = position;
				if (lastPos == _miniMove({0, 0, zAdd})) {
					zEnd = true;
					z = 0;
				}
			}
			if (!zEnd)
				z += zAdd;
		}
	}

	if (beforePosition != position) {
		// if moving, update front vector
		front = glm::normalize(position - beforePosition);
	}

	return position;
}

// -- Private methods ----------------------------------------------------------

/**
 * @brief Move in a really short distance -> called by ACharecter::_moveTo
 *
 * @param movement The movement to do (final dest = position + movement)
 * @return glm::vec3 The final position (start position if move is impossible)
 */
glm::vec3	ACharacter::_miniMove(glm::vec3 movement) {
	glm::vec3 pos = getPos() + movement;
	if (position == pos)
		return position;
	if (_canMoveOnFromTo(position, pos)) {  // if we can move
		position = pos;
		return position;
	}
	return position;
}

// -- Exceptions errors --------------------------------------------------------

ACharacter::ACharacterException::ACharacterException()
: std::runtime_error("ACharacter Exception") {}

ACharacter::ACharacterException::ACharacterException(const char* whatArg)
: std::runtime_error(std::string(std::string("ACharacterError: ") + whatArg).c_str()) {}
