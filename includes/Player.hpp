#ifndef PLAYER_HPP_
#define PLAYER_HPP_

#include <iostream>
#include <stdexcept>
#include "ACharacter.hpp"

class Player : public ACharacter {
private:
	// Members

public:
	// Constructors
	Player();
	~Player();
	Player(Player const &src);

	// Operators
	Player &operator=(Player const &rhs);

	// Methods
	bool	update(float d_time);
	bool	draw();

	// Exceptions
	class PlayerException : public std::runtime_error {
	public:
		PlayerException();
		explicit PlayerException(const char* what_arg);
	};
};

#endif  // PLAYER_HPP_
