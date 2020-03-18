#include "Player.hpp"
#include "Inputs.hpp"

// -- Constructors -------------------------------------------------------------

Player::Player(SceneGame &game) : ACharacter(game) {
	type = Type::PLAYER;
	name = "Player";
	initParams();
}

Player::~Player() {
}

Player::Player(Player const &src) : ACharacter(src) {
	*this = src;
}

// -- Operators ----------------------------------------------------------------

Player &Player::operator=(Player const &rhs) {
	if ( this != &rhs ) {
		ACharacter::operator=(rhs);
		bombs = rhs.bombs;
		_invulnerable = rhs._invulnerable;
		_toDraw = rhs._toDraw;
		passFire = rhs.passFire;
		passWall = rhs.passWall;
		detonator = rhs.detonator;
		passBomb = rhs.passBomb;
		bombProgation = rhs.bombProgation;
	}
	return *this;
}

// -- Methods ------------------------------------------------------------------

/**
 * @brief Initial value for player.
 *
 */
void	Player::initParams() {
	bombs = 1;
	speed = 5;
	alive = true;
	lives = 2;
	_invulnerable = 3.0f;
	_toDraw = 0;
}

/**
 * @brief update is called each frame.
 *
 * @param dTime Delta Time
 * @return true if success
 * @return false if failure
 */
bool	Player::update(float const dTime) {
	if (!active)
		return true;
	if (alive) {
		if (_invulnerable > 0.0f)
			_invulnerable -= dTime;
		if (_invulnerable < 0.0f)
			_invulnerable = 0.0f;
		_move(dTime);
		if (Inputs::getKeyDown(InputType::ACTION)) {
			_putBomb();
		}
	} else {
		logDebug("Player is dead.")
		game.state = GameState::GAME_OVER;
	}
	return true;
}

/**
 * @brief draw is called each frame.
 *
 * @return true if success
 * @return false if failure
 */
bool	Player::draw(Gui &gui) {
	if (_invulnerable > 0) {
		_toDraw = ((_toDraw + 1) % 10);
		if (_toDraw > 5)
			return true;
	}
	gui.drawCube(Block::PLAYER, getPos());
	return true;
}

/**
 * @brief Player Take <damage> damages.
 *
 * @param damage
 * @return true if damage taken
 * @return false if damage not taken
 */
bool	Player::takeDamage(const int damage) {
	if (_invulnerable <= 0.0f) {
		if (ACharacter::takeDamage(damage)) {
			_invulnerable = 3.0f;
		}
	}
	return false;
}

/**
 * @brief Player take a <bonus> which allow to power up.
 *
 * @param bonus
 * @return true
 * @return false
 */
bool	Player::takeBonus(BonusType::Enum bonus) {
	switch (bonus) {
		case BonusType::LIFE:
			lives++;
			break;
		case BonusType::BOMBS:
			bombs++;
			break;
		case BonusType::FLAMES:
			bombProgation++;
			break;
		case BonusType::SPEED:
			speed = (speed >= 4) ? 4 : ++speed;
			break;
		case BonusType::WALLPASS:
			passWall = true;
			break;
		case BonusType::DETONATOR:
			detonator = true;
			break;
		case BonusType::BOMBPASS:
			passBomb = true;
			break;
		case BonusType::FLAMPASS:
			passFire = true;
			break;
		case BonusType::SHIELD:
			_invulnerable += 5.0f;
			break;
		default:
			break;
	}
	return true;
}

// -- Private Methods ----------------------------------------------------------

void	Player::_move(float const dTime) {
	std::unordered_set<AEntity *>	collisions;

	if (Inputs::getKey(InputType::UP)) {
		_moveTo(Dirrection::UP, dTime);
	}
	if (Inputs::getKey(InputType::RIGHT)) {
		_moveTo(Dirrection::RIGHT, dTime);
	}
	if (Inputs::getKey(InputType::DOWN)) {
		_moveTo(Dirrection::DOWN, dTime);
	}
	if (Inputs::getKey(InputType::LEFT)) {
		_moveTo(Dirrection::LEFT, dTime);
	}
	collisions = getCollision(position);
	_clearCollisionObjects(collisions);
}

void	Player::_putBomb() {
	if (bombs <= 0)
		return;
	if (game.board[position.x + 0.5][position.z + 0.5].size() == 0) {
		Bomb	*bomb = new Bomb(game);
		game.board[position.x + 0.5][position.z + 0.5].push_back(bomb);
		_noCollisionObjects.insert(bomb);
		bombs -= 1;
	}
}

// -- Exceptions errors --------------------------------------------------------

Player::PlayerException::PlayerException()
: std::runtime_error("Player Exception") {}

Player::PlayerException::PlayerException(const char* whatArg)
: std::runtime_error(std::string(std::string("PlayerError: ") + whatArg).c_str()) {}
