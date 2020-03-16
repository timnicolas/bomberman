#include "End.hpp"
#include "SceneGame.hpp"

// -- Constructors -------------------------------------------------------------

End::End(SceneGame &game) : AObject(game) {
	type = Type::END;
	name = "End";
	_texture = Block::END;
}
\
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
	(void)dTime;
	if (game.flags <= 0)
		_texture = Block::END_OPEN;
	return true;
}

/**
 * @brief draw is called each frame.
 *
 * @return true if success
 * @return false if failure
 */
bool	End::draw(Gui &gui) {
	gui.drawCube(_texture, getPos());
	return true;
}

// -- Exceptions errors --------------------------------------------------------

End::EndException::EndException()
: std::runtime_error("End Exception") {}

End::EndException::EndException(const char* whatArg)
: std::runtime_error(std::string(std::string("EndError: ") + whatArg).c_str()) {}
