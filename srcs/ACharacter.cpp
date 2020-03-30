#include "ACharacter.hpp"
#include "SceneGame.hpp"
#include "useGlm.hpp"

// -- Constructors -------------------------------------------------------------

ACharacter::ACharacter(SceneGame &game)
: AEntity(game),
  size(0.95, 0.95, 0.95)
{
	category = Category::MOBILE;
	lives = 1;
	speed = 1.0;
	position = {0.0, 0.0, 0.0};
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
glm::vec3		ACharacter::getPos() {
	return position;
}

/**
 * @brief Get the current position (in integer)
 *
 * @return glm::ivec2 The integer position
 */
glm::ivec2		ACharacter::getIntPos() const {
	return glm::ivec2(
		static_cast<int>(position.x + 0.5),
		static_cast<int>(position.z + 0.5)
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
 * @brief Character Take <damage> damages.
 *
 * @param damage
 * @return true if damage taken
 * @return false if damage not taken
 */
bool	ACharacter::takeDamage(const int damage) {
	if (!active || !destructible)
		return false;
	lives -= damage;
	if (lives <= 0) {
		lives = 0;
		alive = false;
	}
	return true;
}

/**
 * @brief get a list of entity in collision with the Character at a position.
 *
 * @param pos default VOID_POS3
 * @return std::unordered_set<AEntity *> collisions
 */
std::unordered_set<AEntity *>	ACharacter::getCollision(glm::vec3 dest) {
	if (dest == VOID_POS3) {
		logWarn("dest is VOID_POS3 in ACharacter::getCollision");
		dest = getPos();
	}
	std::unordered_set<AEntity *> collisions;

	/* get all positions blocks under character on a position */
	std::vector<glm::ivec2> allPos;
	glm::ivec2 idest = glm::ivec2(static_cast<int>(dest.x), static_cast<int>(dest.z));
	glm::ivec2 tmpPos;

	allPos.push_back(idest);

	if (static_cast<int>(dest.x + size.x) > idest.x) {
		tmpPos = idest;
		tmpPos.x += 1;
		allPos.push_back(tmpPos);

		if (static_cast<int>(dest.z + size.z) > idest.y) {
			tmpPos = idest;
			tmpPos.x += 1;
			tmpPos.y += 1;
			allPos.push_back(tmpPos);
		}
	}
	if (static_cast<int>(dest.z + size.z) > idest.y) {
		tmpPos = idest;
		tmpPos.y += 1;
		allPos.push_back(tmpPos);
	}

	for (auto && blockPos : allPos) {
		for (auto &&entity : getBoard()[blockPos.x][blockPos.y]) {
			collisions.insert(entity);
		}
	}
	return collisions;
}

/**
 * @brief Test if Character have a collision at position <atPosition>.
 *
 * @param atPosition
 * @param offset
 * @return true if has a collision
 * @return false if no collision
 */
bool	ACharacter::hasCollision(glm::vec3 atPosition, float offset) {
	getPos();
	if (position.x >= atPosition.x - size.x + offset
	&& position.x <= atPosition.x + size.x - offset
	&& position.z >= atPosition.z - size.z + offset
	&& position.z <= atPosition.z + size.z - offset)
		return true;
	return false;
}

// -- Protected Methods --------------------------------------------------------

/**
 * @brief Check if we can walk on a block
 *
 * @param pos The block pos
 * @return true If we can walk on this block
 */
bool ACharacter::_canWalkOnBlock(glm::ivec2 pos) const {
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
bool	ACharacter::_canMoveOn(glm::vec3 dest) {
	/* check if we are on the game board */
	if (game.positionInGame(dest, size) == false) {
		return false;
	}

	/* check colision with all entities under character */
	std::unordered_set<AEntity *> allColisionsEntity = getCollision(dest);
	for (auto && entity : allColisionsEntity) {
		if (!_canWalkOnEntity(entity))
			return false;
	}
	return true;
}

/**
 * @brief Move to direction if possible.
 *
 * @param direction Direction to move
 * @param dTime Delta time
 * @param offset Offset to turn correction (-1 to don't use correction)
 * @return glm::vec3 finale position
 */
glm::vec3	ACharacter::_moveTo(Direction::Enum direction, float const dTime, float const offset) {
	glm::vec3 						pos = getPos();

	switch (direction) {
		case Direction::UP:
			pos.z -= speed * dTime;
			break;
		case Direction::RIGHT:
			pos.x += speed * dTime;
			break;
		case Direction::DOWN:
			pos.z += speed * dTime;
			break;
		case Direction::LEFT:
			pos.x -= speed * dTime;
			break;
		default:
			return position;
	}
	if (game.positionInGame(pos, size)) {
		if (_canMoveOn(pos)) {  // if we can move
			position = pos;
		}
		else if (offset > 0) {  // if we cannot move
			if (direction == Direction::UP || direction == Direction::DOWN) {
				glm::vec3 tmpPos = pos;
				tmpPos.x = static_cast<int>(pos.x);
				if (pos.x - tmpPos.x < offset && _canMoveOn(tmpPos)) {
					// can move up or down
					return _moveTo(Direction::LEFT, dTime, -1);
				}
				tmpPos.x = static_cast<int>(pos.x + 1);
				if (pos.x - tmpPos.x - 1 < offset && _canMoveOn(tmpPos)) {
					// can move up or down
					return _moveTo(Direction::RIGHT, dTime, -1);
				}
			}
			else {  // left | right
				glm::vec3 tmpPos = pos;
				tmpPos.z = static_cast<int>(pos.z);
				if (pos.z - tmpPos.z < offset && _canMoveOn(tmpPos)) {
					// can move left or right
					return _moveTo(Direction::UP, dTime, -1);
				}
				tmpPos.z = static_cast<int>(pos.z + 1);
				if (pos.z - tmpPos.z - 1 < offset && _canMoveOn(tmpPos)) {
					// can move left or right
					return _moveTo(Direction::DOWN, dTime, -1);
				}
			}
		}
	}
	return position;
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

// -- Exceptions errors --------------------------------------------------------

ACharacter::ACharacterException::ACharacterException()
: std::runtime_error("ACharacter Exception") {}

ACharacter::ACharacterException::ACharacterException(const char* whatArg)
: std::runtime_error(std::string(std::string("ACharacterError: ") + whatArg).c_str()) {}
