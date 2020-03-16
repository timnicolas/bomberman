#include "End.hpp"
#include "SceneGame.hpp"
#include "Player.hpp"

// -- Constructors -------------------------------------------------------------

End::End(SceneGame &game) : AObject(game) {
	type = Type::END;
	name = "End";
	_texture = Block::END;
}

End::~End() {
}

End::End(End const &src) : AObject(src) {
	*this = src;
}

// -- Operators ----------------------------------------------------------------

End &End::operator=(End const &rhs) {
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
bool	End::update(float const dTime) {
	(void)dTime;
	if (game.flags <= 0) {
		_texture = Block::END_OPEN;
		crossable = Type::PLAYER;
		std::unordered_set<AEntity *> collisions = _getCollision(0.2f);
		for (auto &&entity : collisions) {
			if (entity->type == Type::PLAYER) {
				game.state = GameState::WIN;
			}
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
bool	End::draw(Gui &gui) {
	gui.drawCube(_texture, getPos());
	return true;
}

// -- Private Methods ----------------------------------------------------------

/**
 * @brief get a list of entity in collision with the End.
 *
 * @param offset default offset = 0.05f. need to be a positive value.
 * @return std::unordered_set<AEntity *> collisions
 */
std::unordered_set<AEntity *>	End::_getCollision(float offset) {
	getPos();
	glm::vec3 player = game.player->getPos();
	std::unordered_set<AEntity *> collisions;
	if (player.x >= position.x - 1.0 + offset
	&& player.x <= position.x + 1.0 - offset
	&& player.z >= position.z - 1.0 + offset
	&& player.z <= position.z + 1.0 - offset)
		collisions.insert(game.player);

	return collisions;
}

// -- Exceptions errors --------------------------------------------------------

End::EndException::EndException()
: std::runtime_error("End Exception") {}

End::EndException::EndException(const char* whatArg)
: std::runtime_error(std::string(std::string("EndError: ") + whatArg).c_str()) {}
