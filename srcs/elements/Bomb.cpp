#include "Bomb.hpp"
#include "Fire.hpp"
#include "SceneGame.hpp"
#include "Player.hpp"

// -- Constructors -------------------------------------------------------------

Bomb::Bomb(SceneGame &game) : AObject(game) {
	type = Type::BOMB;
	name = "Bomb";
	_countdown = 4.0;
	_propagation = 3;
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
	if (!active)
		return true;
	if (_countdown > 0)
		logInfo("countdown:" << _countdown);
	_countdown -= dTime;
	if (_countdown <= 0.0) {
		glm::vec3	pos = getPos();
		explode({pos.x, pos.z});
	}
	return true;
}

/**
 * @brief The bomb explode in the N.E.S.W. directions at _propagation distance.
 *
 * @param pos
 * @return Bomb* this
 */
Bomb	*Bomb::explode(glm::vec2 const pos) {
	int		i;
	std::vector<AEntity *>	box;

	// top
	i = 0;
	while (++i < _propagation) {
		if (!_propagationExplosion({pos.x, pos.y + i}))
			break;
	}
	// right
	i = 0;
	while (++i < _propagation) {
		if (!_propagationExplosion({pos.x + i, pos.y}))
			break;
	}
	// bottom
	i = 0;
	while (++i < _propagation) {
		if (!_propagationExplosion({pos.x, pos.y - i}))
			break;
	}
	// left
	i = 0;
	while (++i < _propagation) {
		if (!_propagationExplosion({pos.x - i, pos.y}))
			break;
	}
	game.board[pos.x][pos.y].push_back(new Fire(game));
	active = false;
	game.player->bombs++;

	return this;
}

/**
 * @brief draw is called each frame.
 *
 * @return true if success
 * @return false if failure
 */
bool	Bomb::draw(Gui &gui) {
	if (active) {
		gui.drawCube(Block::BOMB, getPos());
	}
	return true;
}

// -- Private Methods ----------------------------------------------------------

bool	Bomb::_propagationExplosion(glm::vec2 const place) {
	logDebug("_propagationExplosion");

	if (place.x < 0 || place.y < 0)
		return false;
	std::vector<AEntity *>	&box = game.board[place.x][place.y];
	bool					result = true;

	logDebug("initialisation");
	if (box.size() == 0) {
		logDebug("box.size == 0");
		box.push_back(new Fire(game));
		logInfo("push fire on case [" << place.x << ", " << place.y << "]");
	}
	else {
		logDebug("box.size > 0:" << box.size());
		std::vector<AEntity *>::iterator it = box.begin();
		while(it != box.end()) {
			logDebug("blockPropagation ?");
			if ((*it)->blockPropagation) {
				logDebug("blockPropagation !");
				result = false;
			}
			logDebug("destructible ?");
			if ((*it)->destructible) {
				logDebug("destructible !");
				(*it)->active = false;
				// delete (*it);
				logDebug("clear entity");
				it = game.board[place.x][place.y].erase(it);
				logDebug("clear place on board");
				box.push_back(new Fire(game));
				logDebug("new fire");
				logDebug("destruction !");
			} else {
				++it;
			}
		}
	}
	return result;
}

// -- Exceptions errors --------------------------------------------------------

Bomb::BombException::BombException()
: std::runtime_error("Bomb Exception") {}

Bomb::BombException::BombException(const char* whatArg)
: std::runtime_error(std::string(std::string("BombError: ") + whatArg).c_str()) {}
