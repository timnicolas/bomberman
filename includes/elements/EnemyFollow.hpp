#pragma once

#include <iostream>
#include <stdexcept>
#include <vector>
#include "AEnemy.hpp"
#include "SceneGame.hpp"

/**
 * @brief This is the enemy object
 */
class EnemyFollow : public AEnemy {
private:
	EnemyFollow();
	std::deque<PathNode>	_path;

protected:
	virtual bool	_update(float const dTime);
	virtual bool	_postUpdate();
	virtual bool	_draw(Gui &gui);

public:
	// Members
	int			bombs;

	// Constructors
	explicit EnemyFollow(SceneGame &game);
	~EnemyFollow();
	EnemyFollow(EnemyFollow const &src);

	// Operators
	EnemyFollow &operator=(EnemyFollow const &rhs);

	// Methods
	static EnemyFollow *	generateEnemy(SceneGame &game, float rate);
};
