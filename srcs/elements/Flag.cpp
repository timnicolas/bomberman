#include "Flag.hpp"
#include "SceneGame.hpp"

// -- Constructors -------------------------------------------------------------

Flag::Flag(SceneGame &game) : AObject(game) {
	type = Type::FLAG;
	name = "Flag";
	destructible = true;
	blockPropagation = true;
}

Flag::~Flag() {
	getPos();
	if (game.clearFromBoard(this, {position.x, position.z})) {
		game.flags--;
	}
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
bool	Flag::update(float const dTime) {
	(void)dTime;
	return true;
}

/**
 * @brief draw is called each frame.
 *
 * @return true if success
 * @return false if failure
 */
bool	Flag::draw(Gui &gui) {
	gui.drawCube(Block::FLAG, getPos());
	return true;
}

// -- Exceptions errors --------------------------------------------------------

Flag::FlagException::FlagException()
: std::runtime_error("Flag Exception") {}

Flag::FlagException::FlagException(const char* whatArg)
: std::runtime_error(std::string(std::string("FlagError: ") + whatArg).c_str()) {}
