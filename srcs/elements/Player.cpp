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
 * @brief update is called each frame.
 *
 * @param dTime Delta Time
 * @return true if success
 * @return false if failure
 */
bool	Player::update(float const dTime) {
	logDebug("Last Player updated at " << dTime);
	return true;
}

/**
 * @brief draw is called each frame.
 *
 * @return true if success
 * @return false if failure
 */
bool	Player::draw() {
	return true;
}

// -- Exceptions errors --------------------------------------------------------

Player::PlayerException::PlayerException()
: std::runtime_error("Player Exception") {}

Player::PlayerException::PlayerException(const char* whatArg)
: std::runtime_error(std::string(std::string("PlayerError: ") + whatArg).c_str()) {}
