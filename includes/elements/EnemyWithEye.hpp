#pragma once

#include <iostream>
#include <stdexcept>
#include <vector>
#include "AEnemy.hpp"
#include "SceneGame.hpp"

/**
 * @brief This is an enemy object
 *
 * EnemyWithEye will follow a pattern and go to the player if it can view it
 */
class EnemyWithEye : public AEnemy {
private:
	EnemyWithEye();
	Direction::Enum	_dir;
	Direction::Enum	_playerDir;  // direction of the player (if view it)

protected:
	virtual bool	_update(float const dTime);
	virtual bool	_postUpdate();
	virtual bool	_draw(Gui &gui);

public:
	// Constructors
	explicit EnemyWithEye(SceneGame &game);
	~EnemyWithEye();
	EnemyWithEye(EnemyWithEye const &src);

	// Operators
	EnemyWithEye &operator=(EnemyWithEye const &rhs);
};
