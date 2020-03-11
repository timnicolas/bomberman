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
	if (Inputs::getKeyDown(InputType::ACTION)){
		_putBomb();
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

/**
 * @brief get a list of entity in collision with the Player at position pos.
 *
 * @param pos default VOID_POS3
 * @return std::unordered_set<AEntity *> collisions
 */
std::unordered_set<AEntity *>	Player::getCollision(glm::vec3 pos) {
	if (pos == VOID_POS3)
		pos = getPos();
	std::unordered_set<AEntity *> collisions;

	for (auto &&entity : game.board[pos.x + 0.05][pos.z + 0.05]) {
		collisions.insert(entity);
	}
	for (auto &&entity : game.board[pos.x + 0.95][pos.z + 0.05]) {
		collisions.insert(entity);
	}
	for (auto &&entity : game.board[pos.x + 0.05][pos.z + 0.95]) {
		collisions.insert(entity);
	}
	for (auto &&entity : game.board[pos.x + 0.95][pos.z + 0.95]) {
		collisions.insert(entity);
	}
	return collisions;
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
	std::unordered_set<AEntity *> collisions = getCollision(pos);
	logDebug("There are " << collisions.size() << " collisions");
	if (_canMove(collisions)) {
		position = pos;
		_clearCollisionObjects(collisions);
	}
}

bool	Player::_canMove(std::unordered_set<AEntity *> collisions) {
	for (auto &&entity : collisions) {
		if (_noCollisionObjects.find(entity) != _noCollisionObjects.end())
			continue;
		return false;
	}
	return true;
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

void	Player::_clearCollisionObjects(std::unordered_set<AEntity *> collisions) {
	// clear _noCollisionObjects list
	std::unordered_set<AEntity *>::iterator it = _noCollisionObjects.begin();
	while (it != _noCollisionObjects.end()) {
		if (collisions.find(*it) == collisions.end()) {
			it = _noCollisionObjects.erase(it);
		}
		else {
			it++;
		}
	}
}

// -- Exceptions errors --------------------------------------------------------

Player::PlayerException::PlayerException()
: std::runtime_error("Player Exception") {}

Player::PlayerException::PlayerException(const char* whatArg)
: std::runtime_error(std::string(std::string("PlayerError: ") + whatArg).c_str()) {}
