#ifndef WALL_HPP_
#define WALL_HPP_

#include <iostream>
#include <stdexcept>
#include "AObject.hpp"

class Wall : public AObject {
private:
	Wall();
	// Members

public:
	// Constructors
	explicit Wall(SceneGame &game);
	~Wall();
	Wall(Wall const &src);

	// Operators
	Wall &operator=(Wall const &rhs);

	// Methods
	bool	update(float const dTime);
	bool	draw(Gui &gui);

	// Exceptions
	class WallException : public std::runtime_error {
	public:
		WallException();
		explicit WallException(const char* whatArg);
	};
};

#endif  // WALL_HPP_
