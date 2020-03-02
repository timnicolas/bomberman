#ifndef BOMB_HPP_
#define BOMB_HPP_

#include <iostream>
#include <stdexcept>
#include "AObject.hpp"

class Bomb : public AObject {
private:
	// Members

public:
	// Constructors
	Bomb();
	~Bomb();
	Bomb(Bomb const &src);

	// Operators
	Bomb &operator=(Bomb const &rhs);

	// Methods
	bool	update(float d_time);
	bool	draw();

	// Exceptions
	class BombException : public std::runtime_error {
	public:
		BombException();
		explicit BombException(const char* what_arg);
	};
};

#endif  // BOMB_HPP_
