#include "Wall.hpp"

// -- Constructors -------------------------------------------------------------

/**
 * @brief Construct a new Wall:: Wall object
 *
 * @param game A reference to the main SceneGame
 * @param blockType The type of block to create
 */
Wall::Wall(SceneGame &game, Block::Enum blockType)
: AObject(game), _blockType(blockType)
{
	type = Type::WALL;
	name = WALL_STR;
}

/**
 * @brief Destroy the Wall:: Wall object
 */
Wall::~Wall() {
}

/**
 * @brief Construct a new Wall:: Wall object
 *
 * @param src A Wall object (to copy)
 */
Wall::Wall(Wall const &src) : AObject(src) {
	*this = src;
}

// -- Operators ----------------------------------------------------------------

/**
 * @brief Copy operator
 *
 * @param rhs The object to copy
 * @return Wall& A reference to the new object
 */
Wall &Wall::operator=(Wall const &rhs) {
	if (this != &rhs) {
		AObject::operator=(rhs);
		_blockType = rhs._blockType;
	}
	return *this;
}

// -- Methods ------------------------------------------------------------------

/**
 * @brief update is called each frame.
 *
 * @return true if success
 * @return false if failure
 */
bool	Wall::update() {
	return true;
}

/**
 * @brief draw is called each frame.
 *
 * @return true if success
 * @return false if failure
 */
bool	Wall::draw(Gui &gui) {
	gui.drawCube(_blockType, getPos());
	return true;
}

// -- Exceptions errors --------------------------------------------------------

Wall::WallException::WallException()
: std::runtime_error("Wall Exception") {}

Wall::WallException::WallException(const char* whatArg)
: std::runtime_error(std::string(std::string("WallError: ") + whatArg).c_str()) {}
