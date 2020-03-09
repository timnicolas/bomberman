#include "Player.hpp"

// -- Constructors -------------------------------------------------------------

Player::Player(SceneGame &game) : ACharacter(game) {
	type = Type::PLAYER;
	bombs = 1;
	name = "Player";
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
 * update is called each frame.
 * @param d_time Delta Time
 */
bool	Player::update(std::chrono::milliseconds d_time) {
	std::cout << "Last Player updated at " << d_time.count() << std::endl;
	return true;
}

/**
 * draw is called each frame.
 */
bool	Player::draw() {
	return true;
}

bool	Player::isDestructable() {
	return true;
}
bool	Player::blockPropagation() {
	return false;
}
bool	Player::isAlive() {
	return lives;
}

// -- Exceptions errors --------------------------------------------------------

Player::PlayerException::PlayerException()
: std::runtime_error("Player Exception") {}

Player::PlayerException::PlayerException(const char* what_arg)
: std::runtime_error(std::string(std::string("PlayerError: ") + what_arg).c_str()) {}
