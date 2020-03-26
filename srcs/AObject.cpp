#include "AObject.hpp"
#include "SceneGame.hpp"

// -- Constructors -------------------------------------------------------------

AObject::AObject(SceneGame &game)
: AEntity(game),
  isInFlyBoard(false)
{
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
 *
 * @param pos default value VOID_POS3
 */
void		AObject::setPos(glm::vec3 pos) {
	if (pos == glm::vec3(VOID_POS3)) {
		uint8_t i = 0;
		float height = (isInFlyBoard) ? 1 : 0;
		for (auto &&board_it0 : getBoard()) {
			uint8_t j = 0;
			for (auto &&board_it1 : board_it0) {
				for (AEntity *board_it2 : board_it1) {
					if (board_it2 == this) {
						this->position = {i, height, j};
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
		this->position = pos;
	}
}

/**
 * @brief Get the position of the current AObject in the SceneGame::board of
 * this->game.
 *
 * @return glm::vec2
 */
glm::vec3	AObject::getPos() {
	if (position == glm::vec3(VOID_POS3)) {
		setPos();
	}
	return position;
}

/**
 * @brief TakeDamage put an object to die.
 *
 * @param damage
 * @return true if damage taken
 * @return false if damage not taken
 */
bool	AObject::takeDamage(const int damage) {
	if (!active || !destructible || damage <= 0)
		return false;
	alive = false;
	return true;
}

/**
 * @brief Get the board (game.board or game.boardFly)
 *
 * @return std::vector< std::vector< std::vector<AEntity *> > >& A reference to the board
 */
std::vector< std::vector< std::vector<AEntity *> > > & AObject::getBoard() {
	if (isInFlyBoard)
		return game.boardFly;
	return game.board;
}

// -- Exceptions errors --------------------------------------------------------

AObject::AObjectException::AObjectException()
: std::runtime_error("AObject Exception") {}

AObject::AObjectException::AObjectException(const char* whatArg)
: std::runtime_error(std::string(std::string("AObjectError: ") + whatArg).c_str()) {}
