#pragma once

#include <iostream>
#include <stdexcept>
#include <vector>
#include "AEnemy.hpp"
#include "SceneGame.hpp"

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

public:
	// Constructors
	explicit EnemyCrispy(SceneGame &game);
	~EnemyCrispy();
	EnemyCrispy(EnemyCrispy const &src);

	// Operators
	EnemyCrispy &operator=(EnemyCrispy const &rhs);
};
