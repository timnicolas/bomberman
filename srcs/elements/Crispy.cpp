#include "Crispy.hpp"

// -- Constructors -------------------------------------------------------------

Crispy::Crispy(SceneGame &game) : AObject(game) {
	type = Type::CRISPY;
	name = "Crispy";
	destructible = true;
}

Crispy::~Crispy() {
}

Crispy::Crispy(Crispy const &src) : AObject(src) {
	*this = src;
}

// -- Operators ----------------------------------------------------------------

Crispy &Crispy::operator=(Crispy const &rhs) {
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
bool	Crispy::update(float const dTime) {
	logDebug("Last Crispy updated at " + std::to_string(dTime));
	return true;
}

/**
 * @brief draw is called each frame.
 *
 * @return true if success
 * @return false if failure
 */
bool	Crispy::draw(Gui &gui) {
	gui.drawCube(Block::DESTRUCTIBLE_WALL, getPos());
	return true;
}

// -- Exceptions errors --------------------------------------------------------

Crispy::CrispyException::CrispyException()
: std::runtime_error("Crispy Exception") {}

Crispy::CrispyException::CrispyException(const char* whatArg)
: std::runtime_error(std::string(std::string("CrispyError: ") + whatArg).c_str()) {}
