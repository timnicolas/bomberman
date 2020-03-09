#ifndef BOMB_HPP_
#define BOMB_HPP_

#include <iostream>
#include <stdexcept>
#include "AObject.hpp"

class Bomb : public AObject {
private:
	Bomb();
	// Members

public:
	// Constructors
	explicit Bomb(SceneGame &game);
	~Bomb();
	Bomb(Bomb const &src);

	// Operators
	Bomb &operator=(Bomb const &rhs);

	// Methods
	bool	update(std::chrono::milliseconds dTime);
	bool	draw();
	bool	isDestructable();
	bool	blockPropagation();

	// Exceptions
	class BombException : public std::runtime_error {
	public:
		BombException();
		explicit BombException(const char* whatArg);
	};
};

#endif  // BOMB_HPP_
