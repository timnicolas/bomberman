#include "ACharacter.hpp"
#include "SceneGame.hpp"

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
		position = rhs.position;
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
 * @brief Init the Class. Needed to be called before any usage of the Class.
 *
 * @param pos
 * @return ACharacter*
 */
ACharacter		*ACharacter::init(glm::vec3 pos) {
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

void	ACharacter::takeDamage(const int damage) {
	if (!destructible)
		return;
	lives -= damage;
	if (lives <= 0) {
		lives = 0;
		alive = false;
	}
}

/**
 * @brief get a list of entity in collision with the Player at position pos.
 *
 * @param pos default VOID_POS3
 * @return std::unordered_set<AEntity *> collisions
 */
std::unordered_set<AEntity *>	ACharacter::getCollision(glm::vec3 pos) {
	if (pos == VOID_POS3)
		pos = getPos();
	std::unordered_set<AEntity *> collisions;

	for (auto &&entity : game.board[pos.x + 0.05][pos.z + 0.05]) {
		collisions.insert(entity);
	}
	for (auto &&entity : game.board[pos.x + 0.95][pos.z + 0.05]) {
		collisions.insert(entity);
	}
	for (auto &&entity : game.board[pos.x + 0.05][pos.z + 0.95]) {
		collisions.insert(entity);
	}
	for (auto &&entity : game.board[pos.x + 0.95][pos.z + 0.95]) {
		collisions.insert(entity);
	}
	return collisions;
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

void	ACharacter::_clearCollisionObjects(std::unordered_set<AEntity *> collisions) {
	// clear _noCollisionObjects list
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
		if (entity->type == Type::FIRE)
			continue;
		return false;
	}
	return true;
}

/**
 * @brief Move to direction if possible.
 *
 * @param direction
 * @param dTime
 * @return glm::vec3 finale position
 */
glm::vec3	ACharacter::_moveTo(Dirrection::Enum direction, float const dTime) {
	glm::vec3 						pos = getPos();
	std::unordered_set<AEntity *>	collisions;

	switch (direction) {
	case Dirrection::UP:
		pos.z -= speed * dTime;
		break;
	case Dirrection::RIGHT:
		pos.x += speed * dTime;
		break;
	case Dirrection::DOWN:
		pos.z += speed * dTime;
		break;
	case Dirrection::LEFT:
		pos.x -= speed * dTime;
		break;
	}
	if (_canMove(getCollision(pos)))
		position = pos;
	return position;
}

// -- Exceptions errors --------------------------------------------------------

ACharacter::ACharacterException::ACharacterException()
: std::runtime_error("ACharacter Exception") {}

ACharacter::ACharacterException::ACharacterException(const char* whatArg)
: std::runtime_error(std::string(std::string("ACharacterError: ") + whatArg).c_str()) {}
