#pragma once

#include <iostream>
#include <stdexcept>
#include <vector>
#include "AEnemy.hpp"
#include "SceneGame.hpp"

#define ENEMY_CRISPY_STR "enemyCrispy"
#define TIME_BEFORE_TRANSFORM_TO_WALL	8000

/**
 * @brief This is an enemy object
 *
 * EnemyCrispy is a crispy wall. If the enemy can show the player, it transform to a crispy wall
 */
class EnemyCrispy : public AEnemy {
private:
	EnemyCrispy();

	bool			_isWall;
	Direction::Enum	_playerDir;  // direction of the player (if view it)
	int64_t			_lastPayerSeenMs;  // last time that a player has been seen

protected:
	virtual bool	_update();
	virtual bool	_postUpdate();
	virtual bool	_draw(Gui &gui);
	bool			_isPlayerClose(int distance);

	void			_updateModel();
	void			_updateAnimationState();

public:
	// Constructors
	explicit EnemyCrispy(SceneGame &game);
	~EnemyCrispy();
	EnemyCrispy(EnemyCrispy const &src);

	// Operators
	EnemyCrispy &operator=(EnemyCrispy const &rhs);

	// Methods
	virtual bool	init();
	virtual void	animEndCb(std::string animName);
	void			setIdlePos();
};
