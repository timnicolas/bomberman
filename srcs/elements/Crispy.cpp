#include "Crispy.hpp"
#include "SceneGame.hpp"
#include "Player.hpp"
#include "Bonus.hpp"

// -- Constructors -------------------------------------------------------------

Crispy::Crispy(SceneGame &game) : AObject(game) {
	type = Type::CRISPY;
	name = CRISPY_STR;
	destructible = true;
}

Crispy::~Crispy() {
	game.clearFromBoard(this, {position.x, position.z});
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
 * @return true if success
 * @return false if failure
 */
bool	Crispy::update() {
	if (!alive)
		active = false;
	return true;
}

bool	Crispy::postUpdate() {
	if (!active) {
		Bonus	*bonus = Bonus::generateBonus(game, .3f);
		if (bonus != nullptr) {
			game.board[position.x][position.z].push_back(bonus);
			bonus->init();
		}
		delete this;
		return false;
	}
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

	if (static_cast<uint32_t>(rand() % 100) > genWallPercent)
		return nullptr;
	return new Crispy(game);
}

// -- Exceptions errors --------------------------------------------------------

Crispy::CrispyException::CrispyException()
: std::runtime_error("Crispy Exception") {}

Crispy::CrispyException::CrispyException(const char* whatArg)
: std::runtime_error(std::string(std::string("CrispyError: ") + whatArg).c_str()) {}
