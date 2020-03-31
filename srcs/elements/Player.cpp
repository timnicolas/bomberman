#include "Player.hpp"
#include "Inputs.hpp"

// -- Constructors -------------------------------------------------------------

Player::Player(SceneGame &game) : ACharacter(game) {
	type = Type::PLAYER;
	name = "Player";
	resetParams();
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
		totalBombs = rhs.totalBombs;
		bombs = rhs.bombs;
		invulnerable = rhs.invulnerable;
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
 * @brief Init player for new levels
 *
 * @return true
 * @return false
 */
bool	Player::init() {
	invulnerable = 3.0f;
	bombs = totalBombs;
	return true;
}

/**
 * @brief Set the Entity that the Character can cross
 */
void Player::resetCrossable() {
	ACharacter::resetCrossable();
}

/**
 * @brief Reset values for player.
 *
 */
void	Player::resetParams() {
	totalBombs = 1;
	bombs = totalBombs;
	speed = 3;
	alive = true;
	lives = 2;
	invulnerable = 3.0f;
	_toDraw = 0;
	bombProgation = 3;
	passFire = false;
	passWall = false;
	detonator = false;
	passBomb = false;
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
		if (invulnerable > 0.0f)
			invulnerable -= dTime;
		if (invulnerable < 0.0f)
			invulnerable = 0.0f;
		_move(dTime);
		if (Inputs::getKeyDown(InputType::ACTION)) {
			_putBomb();
		}
	} else {
		logInfo("Player is dead.")
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
	if (invulnerable > 0) {
		_toDraw = ((_toDraw + 1) % 10);
		if (_toDraw > 5)
			return true;
	}
	gui.drawCube(Block::PLAYER, getPos(), size);
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
	if (invulnerable <= 0.0f) {
		if (ACharacter::takeDamage(damage)) {
			invulnerable = 3.0f;
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
			totalBombs++;
			bombs++;
			break;
		case BonusType::FLAMES:
			bombProgation++;
			break;
		case BonusType::SPEED:
			speed++;
			if (speed > MAX_SPEED)
				speed = MAX_SPEED;
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
			invulnerable += 10.0f;
			break;
		case BonusType::TIME:
			game.time -= 15.0f;
			break;
		case BonusType::POINTS:
			game.score += 1500;
			break;
		default:
			break;
	}
	return true;
}

void	Player::addBomb() {
	bombs++;
	if (bombs > totalBombs)
		bombs = totalBombs;
}

// -- Private Methods ----------------------------------------------------------

void	Player::_move(float const dTime) {
	glm::vec3	dir = glm::vec3(0, front.y, 0);

	if (Inputs::getKey(InputType::UP)) {
		dir.z -= 1;
	}
	if (Inputs::getKey(InputType::RIGHT)) {
		dir.x += 1;
	}
	if (Inputs::getKey(InputType::DOWN)) {
		dir.z += 1;
	}
	if (Inputs::getKey(InputType::LEFT)) {
		dir.x -= 1;
	}
	_moveTo(dir, dTime);
}

void	Player::_putBomb() {
	if (bombs <= 0)
		return;
	glm::ivec2 intPos = getIntPos();
	if (game.board[intPos.x][intPos.y].size() == 0) {
		Bomb	*bomb = new Bomb(game);
		bomb->setPropagation(bombProgation);
		game.board[intPos.x][intPos.y].push_back(bomb);
		bombs -= 1;
	}
}

// -- Exceptions errors --------------------------------------------------------

Player::PlayerException::PlayerException()
: std::runtime_error("Player Exception") {}

Player::PlayerException::PlayerException(const char* whatArg)
: std::runtime_error(std::string(std::string("PlayerError: ") + whatArg).c_str()) {}
