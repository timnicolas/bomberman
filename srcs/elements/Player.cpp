#include "Player.hpp"
#include "Inputs.hpp"

// -- Constructors -------------------------------------------------------------

Player::Player(SceneGame &game) : ACharacter(game) {
	type = Type::PLAYER;
	bombs = 20;
	name = "Player";
	speed = 5;
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
bool	Player::update(float const dTime) {
	if (!active)
		return true;
	if (alive) {
		_move(dTime);
		if (Inputs::getKeyDown(InputType::ACTION)){
			_putBomb();
		}
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
	gui.drawCube(Block::PLAYER, getPos());
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
	logDebug("There are " << collisions.size() << " collisions");
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
