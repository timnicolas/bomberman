#include "AObject.hpp"
#include "SceneGame.hpp"

// -- Constructors -------------------------------------------------------------

AObject::AObject(SceneGame &game) : AEntity(game) {
	category = Category::STATIC;
	blockPropagation = true;
	destructible = false;
}

AObject::~AObject() {
}

AObject::AObject(AObject const &src) : AEntity(src) {
	*this = src;
}

// -- Operators ----------------------------------------------------------------

AObject &AObject::operator=(AObject const &rhs) {
	if ( this != &rhs ) {
		AEntity::operator=(rhs);
	}
	return *this;
}

// -- Methods ------------------------------------------------------------------

/**
 * @brief Set the position of the object according of his position on the board.
 */
void		AObject::setPos(glm::vec3 pos) {
	if (pos == glm::vec3(VOID_POS3)) {
		uint8_t i = 0;
		for (auto &&board_it0 : game.board) {
			uint8_t j = 0;
			for (auto &&board_it1 : board_it0) {
				for (AEntity *board_it2 : board_it1) {
					if (board_it2 == this) {
						this->pos = {i, 0, j};
						return;
					}
				}
				j++;
			}
			i++;
		}
		throw AObjectException("Impossible to found the position of the object");
	}
	else {
		this->pos = pos;
	}
}

/**
 * @brief Get the position of the current AObject in the SceneGame::board of
 * this->game.
 *
 * @return glm::vec2
 */
glm::vec3	AObject::getPos() {
	if (pos == glm::vec3(VOID_POS3)) {
		setPos();
	}
	return pos;
}

/**
 * @brief
 *
 * @param damage
 */
void	AObject::takeDamage(const int damage) {
	(void)damage;
	if (!destructible)
		return;

	alive = false;
}

// -- Exceptions errors --------------------------------------------------------

AObject::AObjectException::AObjectException()
: std::runtime_error("AObject Exception") {}

AObject::AObjectException::AObjectException(const char* whatArg)
: std::runtime_error(std::string(std::string("AObjectError: ") + whatArg).c_str()) {}
