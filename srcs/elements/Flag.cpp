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
 * @brief update is called each frame.
 *
 * @param dTime Delta Time
 * @return true if success
 * @return false if failure
 */
bool	Flag::update(std::chrono::milliseconds dTime) {
	std::cout << "Last Flag updated at " << dTime.count() << std::endl;
	return true;
}

/**
 * @brief draw is called each frame.
 *
 * @return true if success
 * @return false if failure
 */
bool	Flag::draw() {
	return true;
}

/**
 * @brief A Flag is destructible. This method always return true.
 *
 * @return true
 */
bool	Flag::isDestructable() {
	return true;
}

/**
 * @brief A Flag blocks the propagation. This method always return true.
 *
 * @return true
 */
bool	Flag::blockPropagation() {
	return true;
}

// -- Exceptions errors --------------------------------------------------------

Flag::FlagException::FlagException()
: std::runtime_error("Flag Exception") {}

Flag::FlagException::FlagException(const char* whatArg)
: std::runtime_error(std::string(std::string("BombError: ") + whatArg).c_str()) {}
