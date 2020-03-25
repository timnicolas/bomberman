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

// -- Protected Methods --------------------------------------------------------
bool	Player::_canMove(std::unordered_set<AEntity *> collisions) {
	for (auto &&entity : collisions) {
		if (_noCollisionObjects.find(entity) != _noCollisionObjects.end())
			continue;
		if (entity->crossable == Type::ALL || entity->crossable == type)
			continue;
		if (passWall && entity->type == Type::CRISPY)
			continue;
		if (passBomb && entity->type == Type::BOMB)
			continue;
		return false;
	}
	return true;
}


// -- Private Methods ----------------------------------------------------------

void	Player::_move(float const dTime) {
	std::unordered_set<AEntity *>	collisions;

	if (Inputs::getKey(InputType::UP)) {
		_moveTo(Direction::UP, dTime);
	}
	if (Inputs::getKey(InputType::RIGHT)) {
		_moveTo(Direction::RIGHT, dTime);
	}
	if (Inputs::getKey(InputType::DOWN)) {
		_moveTo(Direction::DOWN, dTime);
	}
	if (Inputs::getKey(InputType::LEFT)) {
		_moveTo(Direction::LEFT, dTime);
	}
	collisions = getCollision(position);
	_clearCollisionObjects(collisions);
}

void	Player::_putBomb() {
	if (bombs <= 0)
		return;
	glm::ivec2 intPos = getIntPos();
	if (game.board[intPos.x][intPos.y].size() == 0) {
		Bomb	*bomb = new Bomb(game);
		bomb->setPropagation(bombProgation);
		// game.board[position.x + 0.5][position.z + 0.5].push_back(bomb);
		game.board[intPos.x][intPos.y].push_back(bomb);
		_noCollisionObjects.insert(bomb);
		bombs -= 1;
	}
}

// -- Exceptions errors --------------------------------------------------------

Player::PlayerException::PlayerException()
: std::runtime_error("Player Exception") {}

Player::PlayerException::PlayerException(const char* whatArg)
: std::runtime_error(std::string(std::string("PlayerError: ") + whatArg).c_str()) {}
