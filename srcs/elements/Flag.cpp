#include "Flag.hpp"

// -- Constructors -------------------------------------------------------------

Flag::Flag(SceneGame &game) : AObject(game) {
	type = Type::FLAG;
	name = "Flag";
}

Flag::~Flag() {
}

Flag::Flag(Flag const &src) : AObject(src) {
	*this = src;
}

// -- Operators ----------------------------------------------------------------

Flag &Flag::operator=(Flag const &rhs) {
	if ( this != &rhs ) {
		AObject::operator=(rhs);
	}
	return *this;
}

// -- Methods ------------------------------------------------------------------

/**
 * update is called each frame.
 * @param d_time Delta Time
 */
bool	Flag::update(std::chrono::milliseconds d_time) {
	std::cout << "Last Flag updated at " << d_time.count() << std::endl;
	return true;
}

/**
 * draw is called each frame.
 */
bool	Flag::draw() {
	return true;
}

bool	Flag::isDestructable() {
	return false;
}

bool	Flag::blockPropagation() {
	return false;
}

// -- Exceptions errors --------------------------------------------------------

Flag::FlagException::FlagException()
: std::runtime_error("Flag Exception") {}

Flag::FlagException::FlagException(const char* what_arg)
: std::runtime_error(std::string(std::string("BombError: ") + what_arg).c_str()) {}
