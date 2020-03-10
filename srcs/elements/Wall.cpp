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
 * @brief update is called each frame.
 *
 * @param dTime Delta Time
 * @return true if success
 * @return false if failure
 */
bool	Wall::update(float const dTime) {
	logDebug("Last Wall updated at " + std::to_string(dTime));
	return true;
}

/**
 * @brief draw is called each frame.
 *
 * @return true if success
 * @return false if failure
 */
bool	Wall::draw() {
	return true;
}

// -- Exceptions errors --------------------------------------------------------

Wall::WallException::WallException()
: std::runtime_error("Wall Exception") {}

Wall::WallException::WallException(const char* whatArg)
: std::runtime_error(std::string(std::string("WallError: ") + whatArg).c_str()) {}
