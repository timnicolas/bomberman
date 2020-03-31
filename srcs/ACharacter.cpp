#include "ACharacter.hpp"
#include "SceneGame.hpp"
#include "useGlm.hpp"

// -- Constructors -------------------------------------------------------------

ACharacter::ACharacter(SceneGame &game) : AEntity(game) {
	category = Category::MOBILE;
	lives = 1;
	speed = 1.0;
	position = {0.0, 0.0, 0.0};
	blockPropagation = false;
	destructible = true;
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
		_noCollisionObjects = rhs._noCollisionObjects;
	}
	return *this;
}

// -- Methods ------------------------------------------------------------------

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
 * @brief get a list of entity in collision with the Character at position pos.
 *
 * @param pos default VOID_POS3
 * @param offset default offset = 0.05f. need to be a positive value.
 * @return std::unordered_set<AEntity *> collisions
 */
std::unordered_set<AEntity *>	ACharacter::getCollision(glm::vec3 pos, float offset) {
	if (pos == VOID_POS3)
		pos = getPos();
	std::unordered_set<AEntity *> collisions;

	for (auto &&entity : getBoard()[pos.x + offset][pos.z + offset]) {
		collisions.insert(entity);
	}
	for (auto &&entity : getBoard()[pos.x + 1.0 - offset][pos.z + offset]) {
		collisions.insert(entity);
	}
	for (auto &&entity : getBoard()[pos.x + offset][pos.z + 1.0 - offset]) {
		collisions.insert(entity);
	}
	for (auto &&entity : getBoard()[pos.x + 1.0 - offset][pos.z + 1.0 - offset]) {
		collisions.insert(entity);
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
	if (position.x >= atPosition.x - 1.0 + offset
	&& position.x <= atPosition.x + 1.0 - offset
	&& position.z >= atPosition.z - 1.0 + offset
	&& position.z <= atPosition.z + 1.0 - offset)
		return true;
	return false;
}

/**
 * @brief Clear entity from list of no collision objects
 *
 * @param entity
 * @return true if element cleared.
 * @return false if no element to clear.
 */
bool	ACharacter::clearNoCollisionObjects(AEntity *entity) {
	if (_noCollisionObjects.find(entity) != _noCollisionObjects.end()) {
		_noCollisionObjects.erase(entity);
		return true;
	}
	return false;
}

// -- Protected Methods --------------------------------------------------------

/**
 * @brief clear _noCollisionObjects list if not in <collisions>
 *
 * @param collisions
 */
void	ACharacter::_clearCollisionObjects(std::unordered_set<AEntity *> collisions) {
	std::unordered_set<AEntity *>::iterator it = _noCollisionObjects.begin();
	while (it != _noCollisionObjects.end()) {
		if (collisions.find(*it) == collisions.end()) {
			it = _noCollisionObjects.erase(it);
		}
		else {
			it++;
		}
	}
}

/**
 * @brief Check if there are collisions that cannot be passed.
 *
 * @param collisions list of AEntity collisions
 * @return true
 * @return false
 */
bool	ACharacter::_canMove(std::unordered_set<AEntity *> collisions) {
	for (auto &&entity : collisions) {
		if (_noCollisionObjects.find(entity) != _noCollisionObjects.end())
			continue;
		if (entity->crossable == Type::ALL || entity->crossable == type)
			continue;
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
glm::vec3	ACharacter::_moveTo(Direction::Enum direction, float const offset,
float ignoreColisions) {
	glm::vec3 						pos = getPos();
	std::unordered_set<AEntity *>	collisions;

	// update status on first move
	if (_entityStatus.status != EntityStatus::RUNNING) {
		setStatus(EntityStatus::RUNNING);
	}

	switch (direction) {
		case Direction::UP:
			pos.z -= speed * game.getDtTime();
			break;
		case Direction::RIGHT:
			pos.x += speed * game.getDtTime();
			break;
		case Direction::DOWN:
			pos.z += speed * game.getDtTime();
			break;
		case Direction::LEFT:
			pos.x -= speed * game.getDtTime();
			break;
		default:
			return position;
	}

	if (game.positionInGame(glm::vec2{pos.x, pos.z})) {
		if (ignoreColisions || _canMove(getCollision(pos))) {  // if we can move
			position = pos;
		}
		else if (offset > 0) {  // if we cannot move
			if (direction == Direction::UP || direction == Direction::DOWN) {
				glm::vec3 tmpPos = pos;
				tmpPos.x = static_cast<int>(pos.x);
				if (pos.x - tmpPos.x < offset && _canMove(getCollision(tmpPos))) {
					// can move up or down
					return _moveTo(Direction::LEFT, -1);
				}
				tmpPos.x = static_cast<int>(pos.x + 1);
				if (pos.x - tmpPos.x - 1 < offset && _canMove(getCollision(tmpPos))) {
					// can move up or down
					return _moveTo(Direction::RIGHT, -1);
				}
			}
			else {  // left | right
				glm::vec3 tmpPos = pos;
				tmpPos.z = static_cast<int>(pos.z);
				if (pos.z - tmpPos.z < offset && _canMove(getCollision(tmpPos))) {
					// can move left or right
					return _moveTo(Direction::UP, -1);
				}
				tmpPos.z = static_cast<int>(pos.z + 1);
				if (pos.z - tmpPos.z - 1 < offset && _canMove(getCollision(tmpPos))) {
					// can move left or right
					return _moveTo(Direction::DOWN, -1);
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
