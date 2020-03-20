#include "Bonus.hpp"
#include "SceneGame.hpp"
#include "Player.hpp"

// -- Static members -----------------------------------------------------------

std::map<BonusType::Enum, Block::Enum> Bonus::_textures = {
	{ BonusType::LIFE, Block::BONUS_LIFE },
	{ BonusType::BOMBS, Block::BONUS_BOMBS },
	{ BonusType::FLAMES, Block::BONUS_FLAMES },
	{ BonusType::SPEED, Block::BONUS_SPEED },
	{ BonusType::WALLPASS, Block::BONUS_WALLPASS },
	{ BonusType::DETONATOR, Block::BONUS_DETONATOR },
	{ BonusType::BOMBPASS, Block::BONUS_BOMBPASS },
	{ BonusType::FLAMPASS, Block::BONUS_FLAMPASS },
	{ BonusType::SHIELD, Block::BONUS_SHIELD },
};

// -- Constructors -------------------------------------------------------------

Bonus::Bonus(SceneGame &game) : AObject(game) {
	type = Type::BONUS;
	name = "Bonus";
	blockPropagation = false;
	destructible = false;
	crossable = Type::ALL;
	_timeToDie = 10.0f;
	_typeBonus = static_cast<BonusType::Enum>(rand() % BonusType::NB_BONUS);
}

Bonus::~Bonus() {
	getPos();
	game.clearFromBoard(this, {position.x, position.z});
}

Bonus::Bonus(Bonus const &src) : AObject(src) {
	*this = src;
}

// -- Operators ----------------------------------------------------------------

Bonus &Bonus::operator=(Bonus const &rhs) {
	if ( this != &rhs ) {
		AObject::operator=(rhs);
		_typeBonus = rhs._typeBonus;
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
bool	Bonus::update(float const dTime) {
	if (!alive || !active)
		return true;
	_timeToDie -= dTime;
	if (_timeToDie <= 0.0) {
		alive = false;
	}
	getPos();
	if (game.player->hasCollision(position, 0.2f)) {
		game.player->takeBonus(_typeBonus);
		active = false;
	}
	return true;
}

/**
 * @brief postUpdate is called each frame. After update()
 *
 * @return true if success
 * @return false if failure
 */
bool	Bonus::postUpdate() {
	if (!alive || !active) {
		delete this;
	}
	return true;
}

/**
 * @brief draw is called each frame.
 *
 * @return true if success
 * @return false if failure
 */
bool	Bonus::draw(Gui &gui) {
	gui.drawCube(_textures[_typeBonus], getPos());
	return true;
}

/**
 * @brief Static class to generate bonus.
 *
 * @param game
 * @param rate Probability to generate an enemy (between 0 and 1). default: 0.1f
 * @return Bonus* or nullptr if nothing has been generated.
 */
Bonus	*Bonus::generateBonus(SceneGame &game, float rate) {
	if (rate <= 0.0f)
		return nullptr;
	if (rate >= 1.0f)
		return new Bonus(game);
	int		percentRate = rand() % 100;
	if (percentRate > static_cast<int>(rate * 100))
		return nullptr;
	return new Bonus(game);
}

// -- Exceptions errors --------------------------------------------------------

Bonus::BonusException::BonusException()
: std::runtime_error("Bonus Exception") {}

Bonus::BonusException::BonusException(const char* whatArg)
: std::runtime_error(std::string(std::string("BonusError: ") + whatArg).c_str()) {}
