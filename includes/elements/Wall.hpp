#ifndef WALL_HPP_
#define WALL_HPP_

#include <iostream>
#include <stdexcept>
#include "AObject.hpp"

/**
 * @brief This is the wall object
 */
class Wall : public AObject {
private:
	Wall();
	// Members
	// graphic information.
	Block::Enum		_blockType;

public:
	// Constructors
	explicit Wall(SceneGame &game, Block::Enum blockType = Block::DURABLE_WALL);
	~Wall();
	Wall(Wall const &src);

	// Operators
	Wall &operator=(Wall const &rhs);

	// Methods
	bool	update();
	bool	draw(Gui &gui);

	// Exceptions
	class WallException : public std::runtime_error {
	public:
		WallException();
		explicit WallException(const char* whatArg);
	};
};

#endif  // WALL_HPP_
