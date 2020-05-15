#ifndef PLAYER_HPP_
#define PLAYER_HPP_

#include <iostream>
#include <stdexcept>
#include "ACharacter.hpp"
#include "SceneGame.hpp"
#include "Bonus.hpp"
#include "Model.hpp"

#define PLAYER_STR "player"

/**
 * @brief This is the player object
 */
class Player : public ACharacter {
private:
	Player();
	// Members
	int			_toDraw;  /**< To draw */

	// Methods
	void	_move();
	bool	_putBomb();
	void	_updateAnimationState();
	void	_updateBonusActifsTime();

public:
	// Members
	int			totalBombs;  /**< Total bombs */
	int			bombs;  /**< Remain bombs */
	bool		passFire;  /**< Has passfire */
	bool		passWall;  /**< Has wallpass */
	bool		detonator;  /**< Has detonator */
	bool		passBomb;  /**< Has bombpass */
	int			bombProgation;  /**< Bomb propagation distance */
	float		invulnerable;  /**< Is invulnerable (for x time) */

	/**
	 * @brief All active bonus for the player
	 */
	struct ActiveBonus {
		float	life;  /**< Player life */
		float	score;  /**< Player score */
		float	time;  /**< Player time left to do level */
		float	bombs;  /**< Player bombs */
		float	flames;  /**< Player bomb propagation */
		float	speed;  /**< Player speed */
		float	wallpass;  /**< Player wallpass */
		float	detonator;  /**< Player detonator */
		float	bombpass;  /**< Player bombpass */
		float	flampass;  /**< Player flampass */
		float	shield;  /**< Player shield */
	};
	ActiveBonus		activeBonus;  /**< All active bonus for the player */

	// Constructors
	explicit Player(SceneGame &game);
	~Player();
	Player(Player const &src);

	// Operators
	Player &operator=(Player const &rhs);

	// Methods
	virtual bool	init();
	bool			update();
	bool			draw(Gui &gui);
	virtual void	resetCrossable();
	void			resetParams();
	bool			takeDamage(const int damage);
	bool			takeBonus(BonusType::Enum bonus, bool silent = false);
	bool			rmBonus(BonusType::Enum bonus);
	void			addBomb();

	virtual void	animEndCb(std::string animName);

	// Exceptions
	class PlayerException : public std::runtime_error {
	public:
		PlayerException();
		/**
		 * @brief Construct a new Spawner Exception object
		 *
		 * @param whatArg Error message
		 */
		explicit PlayerException(const char* whatArg);
	};
};

#endif  // PLAYER_HPP_
