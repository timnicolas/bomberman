#pragma once

#include <iostream>
#include <stdexcept>
#include <vector>
#include "AEnemy.hpp"
#include "SceneGame.hpp"

/**
 * @brief This is an enemy object
 *
 * EnemyBasic will simply follow a pattern
 */
class EnemyBasic : public AEnemy {
private:
	EnemyBasic();
	std::vector<Direction::Enum>	_directionsOrder;
	uint32_t						_dirIdx;

protected:
	virtual bool	_update(float const dTime);
	virtual bool	_postUpdate();
	virtual bool	_draw(Gui &gui);

public:
	// Constructors
	explicit EnemyBasic(SceneGame &game);
	~EnemyBasic();
	EnemyBasic(EnemyBasic const &src);

	// Operators
	EnemyBasic &operator=(EnemyBasic const &rhs);
};
