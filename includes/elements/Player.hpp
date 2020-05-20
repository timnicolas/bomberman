#ifndef PLAYER_HPP_
#define PLAYER_HPP_

#include <iostream>
#include <stdexcept>
#include "ACharacter.hpp"
#include "SceneGame.hpp"
#include "Bonus.hpp"
#include "Model.hpp"

#define PLAYER_STR "player"
#define P_WALK_SPEED .5f

/**
 * @brief This is the player object
 */
class Player : public ACharacter {
private:
	Player();
	// Members
	int			_toDraw;
	bool		_startWinAnim;
	glm::vec3	_endPos;
	glm::vec3	_endDir;

	// Methods
	void	_move();
	bool	_putBomb();
	void	_updateModel();
	void	_updateAnimationState();
	void	_updateBonusActifsTime();

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

	struct BonusActifs {
		float	life;
		float	score;
		float	time;
		float	bombs;
		float	flames;
		float	speed;
		float	wallpass;
		float	detonator;
		float	bombpass;
		float	flampass;
		float	shield;
	};
	BonusActifs		bonusActifs;

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
	void			startWinAnim();

	virtual void	animEndCb(std::string animName);

	// Exceptions
	class PlayerException : public std::runtime_error {
	public:
		PlayerException();
		explicit PlayerException(const char* whatArg);
	};
};

#endif  // PLAYER_HPP_
