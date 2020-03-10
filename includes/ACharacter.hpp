#ifndef ACHARACTER_HPP_
#define ACHARACTER_HPP_

#include <iostream>
#include <stdexcept>
#include "useGlm.hpp"
#include "AEntity.hpp"

// class SceneGame;

class ACharacter : public AEntity {
private:
	ACharacter();

public:
	// Members
	int			lives;
	float		speed;

	// Constructors
	explicit ACharacter(SceneGame &game);
	virtual ~ACharacter();
	ACharacter(ACharacter const &src);

	// Operators
	ACharacter &operator=(ACharacter const &rhs);

	// Methods
	virtual bool	update(float const dTime) = 0;
	virtual bool	draw(Gui &gui) = 0;
	bool			isAlive();
	glm::vec3		getPos();
	ACharacter		*init(glm::vec3 pos);
	void			takeDamage(const int damage);

	// Exceptions
	class ACharacterException : public std::runtime_error {
	public:
		ACharacterException();
		explicit ACharacterException(const char* whatArg);
	};
};

#endif  // ACHARACTER_HPP_
