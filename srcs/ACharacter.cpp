#include "ACharacter.hpp"

// -- Constructors -------------------------------------------------------------

ACharacter::ACharacter() {
}

ACharacter::~ACharacter() {
}

ACharacter::ACharacter(ACharacter const &src) {
	*this = src;
}

// -- Operators ----------------------------------------------------------------

ACharacter &ACharacter::operator=(ACharacter const &rhs) {
	if ( this != &rhs ) {
		AEntity::operator=(rhs);
	}
	return *this;
}

// -- Exceptions errors --------------------------------------------------------

ACharacter::ACharacterException::ACharacterException()
: std::runtime_error("ACharacter Exception") {}

ACharacter::ACharacterException::ACharacterException(const char* what_arg)
: std::runtime_error(std::string(std::string("ACharacterError: ") + what_arg).c_str()) {}
