#ifndef BOMB_HPP_
#define BOMB_HPP_

#include <iostream>
#include <stdexcept>
#include "AObject.hpp"

#define BOMB_STR "bomb"

/**
 * @brief This is the bomb object
 */
class Bomb : public AObject {
private:
	Bomb();
	// Members
	float	_countdown;  /**< Bomb countdown */
	int		_propagation;  /**< Bomb propagation */

	// Methods
	bool	_propagationExplosion(glm::vec2 const pos);

public:
	// Constructors
	explicit Bomb(SceneGame &game);
	~Bomb();
	Bomb(Bomb const &src);

	// Operators
	Bomb &operator=(Bomb const &rhs);

	// Getters/Setters
	Bomb*	setPropagation(const int propagation);

	// Methods
	bool	update();
	bool	postUpdate();
	void	explode(glm::vec2 const pos);
	bool	draw(Gui &gui);
	bool	takeDamage(const int damage);

	// Exceptions
	class BombException : public std::runtime_error {
	public:
		BombException();
		/**
		 * @brief Construct a new Spawner Exception object
		 *
		 * @param whatArg Error message
		 */
		explicit BombException(const char* whatArg);
	};
};

#endif  // BOMB_HPP_
