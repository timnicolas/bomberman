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
 * update is called each frame.
 * @param dTime Delta Time
 */
bool	End::update(std::chrono::milliseconds dTime) {
	std::cout << "Last End updated at " << dTime.count() << std::endl;
	return true;
}

/**
 * draw is called each frame.
 */
bool	End::draw() {
	return true;
}

bool	End::isDestructable() {
	return false;
}

bool	End::blockPropagation() {
	return false;
}

// -- Exceptions errors --------------------------------------------------------

End::EndException::EndException()
: std::runtime_error("End Exception") {}

End::EndException::EndException(const char* whatArg)
: std::runtime_error(std::string(std::string("EndError: ") + whatArg).c_str()) {}
