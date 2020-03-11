#ifndef PLAYER_HPP_
#define PLAYER_HPP_

#include <iostream>
#include <stdexcept>
#include <unordered_set>
#include "ACharacter.hpp"
#include "SceneGame.hpp"

class Player : public ACharacter {
private:
	Player();
	std::unordered_set<AEntity *>	_noCollisionObjects;

	// Methods
	void	_move(float const dTime);
	bool	_canMove(std::unordered_set<AEntity *> collisions);
	void	_putBomb();
	void	_clearCollisionObjects(std::unordered_set<AEntity *> collisions);

public:
	// Members
	int			bombs;

	// Constructors
	explicit Player(SceneGame &game);
	~Player();
	Player(Player const &src);

	// Operators
	Player &operator=(Player const &rhs);

	// Methods
	bool							update(float const dTime);
	bool							draw(Gui &gui);
	std::unordered_set<AEntity *>	getCollision(glm::vec3 pos = VOID_POS3);

	// Exceptions
	class PlayerException : public std::runtime_error {
	public:
		PlayerException();
		explicit PlayerException(const char* whatArg);
	};
};

#endif  // PLAYER_HPP_
