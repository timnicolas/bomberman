#pragma once

#include <iostream>
#include <stdexcept>
#include <vector>
#include "AEnemy.hpp"
#include "SceneGame.hpp"

/**
 * @brief This is an enemy object
 *
 * EnemyFollow will always try to find the player and follow him
 */
class EnemyFollow : public AEnemy {
private:
	EnemyFollow();

	// player pathfinding info
	bool							_findPlayer;
	std::deque<PathNode>			_path;
	// last calculation of path
	std::chrono::milliseconds		_lastFindMs;

protected:
	virtual bool	_update();
	virtual bool	_postUpdate();
	virtual bool	_draw(Gui &gui);

public:
	// Constructors
	explicit EnemyFollow(SceneGame &game);
	~EnemyFollow();
	EnemyFollow(EnemyFollow const &src);

	// Operators
	EnemyFollow &operator=(EnemyFollow const &rhs);
};
