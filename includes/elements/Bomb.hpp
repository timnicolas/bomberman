#ifndef BOMB_HPP_
#define BOMB_HPP_

#include <iostream>
#include <stdexcept>
#include "AObject.hpp"

class Bomb : public AObject {
private:
	Bomb();
	// Members
	float	_countdown;

	// Methods
	bool	_propagationExplosion(glm::vec2 const pos,
	std::vector<AEntity *> box, int i);

public:
	// Constructors
	explicit Bomb(SceneGame &game);
	~Bomb();
	Bomb(Bomb const &src);

	// Operators
	Bomb &operator=(Bomb const &rhs);

	// Methods
	bool	update(float const dTime);
	Bomb*	explode(glm::vec2 const pos);
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
