#ifndef ENEMY_HPP_
#define ENEMY_HPP_

#include <iostream>
#include <stdexcept>
#include "ACharacter.hpp"
#include "SceneGame.hpp"

/**
 * @brief This is the enemy object
 */
class Enemy : public ACharacter {
private:
	Enemy();
	Dirrection::Enum	_direction;

public:
	// Members
	int			bombs;

	// Constructors
	explicit Enemy(SceneGame &game);
	~Enemy();
	Enemy(Enemy const &src);

	// Operators
	Enemy &operator=(Enemy const &rhs);

	// Methods
	bool			update(float const dTime);
	bool			postUpdate();
	bool			draw(Gui &gui);
	static Enemy*	generateEnemy(SceneGame &game, float rate);

	// Exceptions
	class EnemyException : public std::runtime_error {
	public:
		EnemyException();
		explicit EnemyException(const char* whatArg);
	};
};

#endif  // ENEMY_HPP_
