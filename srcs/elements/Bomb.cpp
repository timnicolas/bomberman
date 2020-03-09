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
 * @param d_time Delta Time
 */
bool	Bomb::update(std::chrono::milliseconds d_time) {
	std::cout << "Last Bomb updated at " << d_time.count() << std::endl;
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

Bomb::BombException::BombException(const char* what_arg)
: std::runtime_error(std::string(std::string("BombError: ") + what_arg).c_str()) {}
