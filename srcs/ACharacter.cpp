#include "ACharacter.hpp"
#include "SceneGame.hpp"

// -- Constructors -------------------------------------------------------------

ACharacter::ACharacter(SceneGame &game) : AEntity(game) {
	category = Category::MOBILE;
	lives = 1;
	speed = 1.0;
	pos = {0.0, 0.0};
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
		pos = rhs.pos;
	}
	return *this;
}

// -- Methods ------------------------------------------------------------------

/**
 * @brief Get the current postion of the ACharacter
 *
 * @return glm::vec2
 */
glm::vec2		ACharacter::getPos() {
	return pos;
}

/**
 * @brief Init the Class. Needed to be called before any usage of the Class.
 *
 * @param pos
 * @return ACharacter*
 */
ACharacter		*ACharacter::init(glm::vec2 pos) {
	this->pos = pos;
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

// -- Exceptions errors --------------------------------------------------------

ACharacter::ACharacterException::ACharacterException()
: std::runtime_error("ACharacter Exception") {}

ACharacter::ACharacterException::ACharacterException(const char* whatArg)
: std::runtime_error(std::string(std::string("ACharacterError: ") + whatArg).c_str()) {}
