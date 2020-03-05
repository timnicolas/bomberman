#ifndef WALL_HPP_
#define WALL_HPP_

#include <iostream>
#include <stdexcept>
#include "AObject.hpp"

class Wall : public AObject {
private:
	// Members

public:
	// Constructors
	Wall();
	~Wall();
	Wall(Wall const &src);

	// Operators
	Wall &operator=(Wall const &rhs);

	// Methods
	bool	update(std::chrono::milliseconds d_time);
	bool	draw();
	bool	isDestructable();
	bool	blockPropagation();

	// Exceptions
	class WallException : public std::runtime_error {
	public:
		WallException();
		explicit WallException(const char* what_arg);
	};
};

#endif  // WALL_HPP_
