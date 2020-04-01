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
	Direction::Enum	_dir;

protected:
	virtual bool	_update();
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
