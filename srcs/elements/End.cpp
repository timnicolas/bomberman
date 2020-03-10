#include "End.hpp"

// -- Constructors -------------------------------------------------------------

End::End(SceneGame &game) : AObject(game) {
	type = Type::END;
	name = "End";
}

End::~End() {
}

End::End(End const &src) : AObject(src) {
	*this = src;
}

// -- Operators ----------------------------------------------------------------

End &End::operator=(End const &rhs) {
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
bool	End::update(float const dTime) {
	logDebug("Last End updated at " << dTime);
	return true;
}

/**
 * @brief draw is called each frame.
 *
 * @return true if success
 * @return false if failure
 */
bool	End::draw() {
	return true;
}

/**
 * @brief The End is indestrutible. This method always return false;
 *
 * @return false
 */
bool	End::isDestructable() {
	return false;
}

/**
 * @brief The End blocks the propagation. This method always return true.
 *
 * @return true
 */
bool	End::blockPropagation() {
	return true;
}

// -- Exceptions errors --------------------------------------------------------

End::EndException::EndException()
: std::runtime_error("End Exception") {}

End::EndException::EndException(const char* whatArg)
: std::runtime_error(std::string(std::string("EndError: ") + whatArg).c_str()) {}
