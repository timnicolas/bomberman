#ifndef BOMB_HPP_
#define BOMB_HPP_

#include <iostream>
#include <stdexcept>
#include "AObject.hpp"

/**
 * @brief This is the bomb object
 */
class Bomb : public AObject {
private:
	Bomb();
	// Members
	float	_countdown;
	int		_propagation;

	// Methods
	bool	_propagationExplosion(glm::vec2 const pos);

public:
	// Constructors
	explicit Bomb(SceneGame &game);
	~Bomb();
	Bomb(Bomb const &src);

	// Operators
	Bomb &operator=(Bomb const &rhs);

	// Methods
	bool	update(float const dTime);
	bool	postUpdate();
	void	explode(glm::vec2 const pos);
	bool	draw(Gui &gui);
	void	takeDamage(const int damage);


	// Exceptions
	class BombException : public std::runtime_error {
	public:
		BombException();
		explicit BombException(const char* whatArg);
	};
};

#endif  // BOMB_HPP_
