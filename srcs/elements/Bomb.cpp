#include "Bomb.hpp"
#include "Fire.hpp"
#include "SceneGame.hpp"

// -- Constructors -------------------------------------------------------------

Bomb::Bomb(SceneGame &game) : AObject(game) {
	type = Type::BOMB;
	name = "Bomb";
	_countdown = 4.0;
}

Bomb::~Bomb() {
}

Bomb::Bomb(Bomb const &src) : AObject(src) {
	*this = src;
}

// -- Operators ----------------------------------------------------------------

Bomb &Bomb::operator=(Bomb const &rhs) {
	if ( this != &rhs ) {
		AObject::operator=(rhs);
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
bool	Bomb::update(float const dTime) {
	_countdown -= dTime;
	if (_countdown <= 0.0) {
		explode(getPos());
	}
	return true;
}

Bomb	*Bomb::explode(glm::vec2 const pos) {
	int		propagation = 5;
	int		i;
	std::vector<AEntity *>	box;

	// top
	i = 0;
	while (++i < propagation) {
		box = game.board[pos.x][pos.y + i];
		if (!_propagationExplosion(pos, box, i))
			break;
	}
	// right
	i = 0;
	while (++i < propagation) {
		box = game.board[pos.x + i][pos.y];
		if (!_propagationExplosion(pos, box, i))
			break;
	}
	// bottom
	i = 0;
	while (++i < propagation) {
		box = game.board[pos.x][pos.y - i];
		if (!_propagationExplosion(pos, box, i))
			break;
	}
	// left
	i = 0;
	while (++i < propagation) {
		box = game.board[pos.x - i][pos.y];
		if (!_propagationExplosion(pos, box, i))
			break;
	}

	return this;
}

bool	Bomb::_propagationExplosion(glm::vec2 const pos, std::vector<AEntity *> box, int i) {
	bool	result = true;
	if (box.size() == 0) {
		box.push_back(new Fire(game));
	}
	else {
		for (std::vector<AEntity *>::iterator it = box.begin();
		it != box.end(); ++it) {
			if ((*it)->blockPropagation) {
				result = false;
			}
			if ((*it)->destructible) {
				game.board[pos.x][pos.y + i].erase(it);
				delete (*it);
				box.push_back(new Fire(game));
			}
		}
	}
	return result;
}

/**
 * @brief draw is called each frame.
 *
 * @return true if success
 * @return false if failure
 */
bool	Bomb::draw() {
	return true;
}

// -- Exceptions errors --------------------------------------------------------

Bomb::BombException::BombException()
: std::runtime_error("Bomb Exception") {}

Bomb::BombException::BombException(const char* whatArg)
: std::runtime_error(std::string(std::string("BombError: ") + whatArg).c_str()) {}
