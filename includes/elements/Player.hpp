#ifndef PLAYER_HPP_
#define PLAYER_HPP_

#include <iostream>
#include <stdexcept>
#include "ACharacter.hpp"
#include "SceneGame.hpp"

class Player : public ACharacter {
public:
	// Members
	int			bombs;
	SceneGame	*game;

	// Constructors
	Player();
	~Player();
	Player(Player const &src);

	// Operators
	Player &operator=(Player const &rhs);

	// Methods
	bool	update(std::chrono::milliseconds d_time);
	bool	draw();
	bool	isDestructable();
	bool	blockPropagation();
	bool	isAlive();

	// Exceptions
	class PlayerException : public std::runtime_error {
	public:
		PlayerException();
		explicit PlayerException(const char* what_arg);
	};
};

#endif  // PLAYER_HPP_
