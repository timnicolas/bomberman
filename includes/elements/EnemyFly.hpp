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
	std::vector<Direction::Enum>	_directionsOrder;
	uint32_t						_dirIdx;

protected:
	virtual bool	_update(float const dTime);
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
