#include "Player.hpp"
#include "Inputs.hpp"

// -- Constructors -------------------------------------------------------------

Player::Player(SceneGame &game) : ACharacter(game) {
	type = Type::PLAYER;
	bombs = 1;
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
	_move(dTime);
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
	glm::vec3 pos = getPos();
	if (Inputs::getKey(InputType::UP)) {
		pos.z -= speed * dTime;
	}
	if (Inputs::getKey(InputType::RIGHT)) {
		pos.x += speed * dTime;
	}
	if (Inputs::getKey(InputType::DOWN)) {
		pos.z += speed * dTime;
	}
	if (Inputs::getKey(InputType::LEFT)) {
		pos.x -= speed * dTime;
	}
	if (_canMove(pos))
		position = pos;
}

bool	Player::_canMove(glm::vec3 pos) {
	if (game.board[pos.x + 0.05][pos.z + 0.05].size() == 0
	&& game.board[pos.x + 0.05][pos.z + 0.95].size() == 0
	&& game.board[pos.x + 0.95][pos.z + 0.05].size() == 0
	&& game.board[pos.x + 0.95][pos.z + 0.95].size() == 0) {
		logDebug("player can move");
		return true;
	}
	logDebug("player cannot move");
	return false;
}

// -- Exceptions errors --------------------------------------------------------

Player::PlayerException::PlayerException()
: std::runtime_error("Player Exception") {}

Player::PlayerException::PlayerException(const char* whatArg)
: std::runtime_error(std::string(std::string("PlayerError: ") + whatArg).c_str()) {}
