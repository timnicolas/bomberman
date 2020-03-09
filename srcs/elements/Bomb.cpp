#include "Bomb.hpp"

// -- Constructors -------------------------------------------------------------

Bomb::Bomb(SceneGame &game) : AObject(game) {
	type = Type::BOMB;
	name = "Bomb";
}

Bomb::~Bomb() {
}

Bomb::Bomb(Bomb const &src) : AObject(src) {
	*this = src;
}

// -- Operators ----------------------------------------------------------------

Bomb &Bomb::operator=(Bomb const &rhs) {
	if ( this != &rhs ) {
		AObject::operator=(rhs);
	}
	return *this;
}

// -- Methods ------------------------------------------------------------------

/**
 * update is called each frame.
 * @param dTime Delta Time
 */
bool	Bomb::update(std::chrono::milliseconds dTime) {
	std::cout << "Last Bomb updated at " << dTime.count() << std::endl;
	return true;
}

/**
 * draw is called each frame.
 */
bool	Bomb::draw() {
	return true;
}

bool	Bomb::isDestructable() {
	return false;
}

bool	Bomb::blockPropagation() {
	return false;
}

// -- Exceptions errors --------------------------------------------------------

Bomb::BombException::BombException()
: std::runtime_error("Bomb Exception") {}

Bomb::BombException::BombException(const char* whatArg)
: std::runtime_error(std::string(std::string("BombError: ") + whatArg).c_str()) {}
