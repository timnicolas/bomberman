#include "Crispy.hpp"
#include "SceneGame.hpp"
#include "Player.hpp"

// -- Constructors -------------------------------------------------------------

Crispy::Crispy(SceneGame &game) : AObject(game) {
	type = Type::CRISPY;
	name = "Crispy";
	destructible = true;
}

Crispy::~Crispy() {
	getPos();
	game.clearFromBoard(this, {position.x, position.z});
	game.player->clearNoCollisionObjects(this);
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
	(void)dTime;
	if (!alive)
		active = false;
	return true;
}

bool	Crispy::postUpdate() {
	if (!active)
		delete this;
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

Crispy * Crispy::generateCrispy(SceneGame &game, uint32_t genWallPercent) {
	if (genWallPercent <= 0)
		return nullptr;
	if (genWallPercent >= 100)
		return new Crispy(game);

	if (rand() % 100 > genWallPercent)
		return nullptr;
	return new Crispy(game);
}

// -- Exceptions errors --------------------------------------------------------

Crispy::CrispyException::CrispyException()
: std::runtime_error("Crispy Exception") {}

Crispy::CrispyException::CrispyException(const char* whatArg)
: std::runtime_error(std::string(std::string("CrispyError: ") + whatArg).c_str()) {}
