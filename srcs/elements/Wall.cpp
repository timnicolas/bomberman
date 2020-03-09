#include "Wall.hpp"

// -- Constructors -------------------------------------------------------------

Wall::Wall(SceneGame &game) : AObject(game) {
	type = Type::BOMB;
	name = "Wall";
}

Wall::~Wall() {
}

Wall::Wall(Wall const &src) : AObject(src) {
	*this = src;
}

// -- Operators ----------------------------------------------------------------

Wall &Wall::operator=(Wall const &rhs) {
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
bool	Wall::update(std::chrono::milliseconds d_time) {
	std::cout << "Last Wall updated at " << d_time.count() << std::endl;
	return true;
}

/**
 * draw is called each frame.
 */
bool	Wall::draw() {
	return true;
}

bool	Wall::isDestructable() {
	return false;
}

bool	Wall::blockPropagation() {
	return true;
}

// -- Exceptions errors --------------------------------------------------------

Wall::WallException::WallException()
: std::runtime_error("Wall Exception") {}

Wall::WallException::WallException(const char* what_arg)
: std::runtime_error(std::string(std::string("WallError: ") + what_arg).c_str()) {}
