#include "ACharacter.hpp"
#include "SceneGame.hpp"

// -- Constructors -------------------------------------------------------------

ACharacter::ACharacter(SceneGame &game) : AEntity(game) {
	category = Category::MOBILE;
	lives = 1;
	speed = 1.0;
	pos = {0.0, 0.0};
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

glm::vec2		ACharacter::getPos() {
	return pos;
}

ACharacter		*ACharacter::init(glm::vec2 pos) {
	this->pos = pos;
	return this;
}

// -- Exceptions errors --------------------------------------------------------

ACharacter::ACharacterException::ACharacterException()
: std::runtime_error("ACharacter Exception") {}

ACharacter::ACharacterException::ACharacterException(const char* whatArg)
: std::runtime_error(std::string(std::string("ACharacterError: ") + whatArg).c_str()) {}
