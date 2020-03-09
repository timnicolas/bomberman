#ifndef ACHARACTER_HPP_
#define ACHARACTER_HPP_

#include <iostream>
#include <stdexcept>
#include "AEntity.hpp"

// class SceneGame;

class ACharacter : public AEntity {
private:
	ACharacter();

public:
	// Members
	int		life;
	float	speed;

	// Constructors
	explicit ACharacter(SceneGame &game);
	virtual ~ACharacter();
	ACharacter(ACharacter const &src);

	// Operators
	ACharacter &operator=(ACharacter const &rhs);

	// Methods
	virtual bool	update(std::chrono::milliseconds d_time) = 0;
	virtual bool	draw() = 0;
	virtual bool	isAlive() = 0;

	// Exceptions
	class ACharacterException : public std::runtime_error {
	public:
		ACharacterException();
		explicit ACharacterException(const char* what_arg);
	};
};

#endif  // ACHARACTER_HPP_
