#include "Fire.hpp"

// -- Constructors -------------------------------------------------------------

Fire::Fire(SceneGame &game) : AObject(game) {
	type = Type::FIRE;
	name = "Fire";
	blockPropagation = false;
	destructible = false;
}

Fire::~Fire() {
}

Fire::Fire(Fire const &src) : AObject(src) {
	*this = src;
}

// -- Operators ----------------------------------------------------------------

Fire &Fire::operator=(Fire const &rhs) {
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
bool	Fire::update(float const dTime) {
	(void)dTime;
	return true;
}

/**
 * @brief draw is called each frame.
 *
 * @return true if success
 * @return false if failure
 */
bool	Fire::draw(Gui &gui) {
	gui.drawCube(Block::FIRE, getPos());
	return true;
}


// -- Exceptions errors --------------------------------------------------------

Fire::FireException::FireException()
: std::runtime_error("Fire Exception") {}

Fire::FireException::FireException(const char* whatArg)
: std::runtime_error(std::string(std::string("FireError: ") + whatArg).c_str()) {}
