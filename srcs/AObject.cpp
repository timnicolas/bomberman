#include "AObject.hpp"

// -- Constructors -------------------------------------------------------------

AObject::AObject() {
}

AObject::~AObject() {
}

AObject::AObject(AObject const &src) {
	*this = src;
}

// -- Operators ----------------------------------------------------------------

AObject &AObject::operator=(AObject const &rhs) {
	if ( this != &rhs ) {
		AEntity::operator=(rhs);
	}
	return *this;
}

// -- Exceptions errors --------------------------------------------------------

AObject::AObjectException::AObjectException()
: std::runtime_error("AObject Exception") {}

AObject::AObjectException::AObjectException(const char* what_arg)
: std::runtime_error(std::string(std::string("AObjectError: ") + what_arg).c_str()) {}
