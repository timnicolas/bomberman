#pragma once

#include <iostream>
#include <stdexcept>
#include <vector>
#include "AEnemy.hpp"
#include "SceneGame.hpp"

/**
 * @brief This is an enemy object
 *
 * EnemyFly will simply follow a pattern in the boardSky
 */
class EnemyFly : public AEnemy {
private:
	EnemyFly();
	Direction::Enum	_dir;

protected:
	virtual bool	_update();
	virtual bool	_postUpdate();
	virtual bool	_draw(Gui &gui);

public:
	// Constructors
	explicit EnemyFly(SceneGame &game);
	~EnemyFly();
	EnemyFly(EnemyFly const &src);

	// Operators
	EnemyFly &operator=(EnemyFly const &rhs);
};
