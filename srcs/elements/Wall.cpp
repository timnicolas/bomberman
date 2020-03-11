#include "Wall.hpp"

// -- Constructors -------------------------------------------------------------

Wall::Wall(SceneGame &game) : AObject(game) {
	type = Type::WALL;
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
bool	Wall::update(std::chrono::milliseconds dTime) {
	std::cout << "Last Wall updated at " << dTime.count() << std::endl;
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

/**
 * @brief A wall is indestructible. This method always return false.
 *
 * @return false
 */
bool	Wall::isDestructable() {
	return false;
}

/**
 * @brief A wall blocks the bomb propagation. This method always return true.
 *
 * @return true
 */
bool	Wall::blockPropagation() {
	return true;
}

// -- Exceptions errors --------------------------------------------------------

Wall::WallException::WallException()
: std::runtime_error("Wall Exception") {}

Wall::WallException::WallException(const char* whatArg)
: std::runtime_error(std::string(std::string("WallError: ") + whatArg).c_str()) {}
