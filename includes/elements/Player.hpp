#ifndef PLAYER_HPP_
#define PLAYER_HPP_

#include <iostream>
#include <stdexcept>
#include "ACharacter.hpp"
#include "SceneGame.hpp"
#include "Bonus.hpp"
#include "Model.hpp"

/**
 * @brief This is the player object
 */
class Player : public ACharacter {
private:
	Player();
	// Members
	int			_toDraw;
	Model		*_model;

	// Methods
	void	_move(float const dTime);
	void	_putBomb();

protected:
	bool	_canMove(std::unordered_set<AEntity *> collisions);

public:
	// Members
	int			totalBombs;
	int			bombs;
	bool		passFire;
	bool		passWall;
	bool		detonator;
	bool		passBomb;
	int			bombProgation;
	float		invulnerable;

	// Constructors
	explicit Player(SceneGame &game);
	~Player();
	Player(Player const &src);

	// Operators
	Player &operator=(Player const &rhs);

	// Methods
	bool							init();
	bool							update(float const dTime);
	bool							draw(Gui &gui);
	void							resetParams();
	bool							takeDamage(const int damage);
	bool							takeBonus(BonusType::Enum bonus);
	void							addBomb();

	// Exceptions
	class PlayerException : public std::runtime_error {
	public:
		PlayerException();
		explicit PlayerException(const char* whatArg);
	};
};

#endif  // PLAYER_HPP_
