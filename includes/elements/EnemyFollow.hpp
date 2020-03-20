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
	// followbasic pattern when cannot go to the player position
	std::vector<Direction::Enum>	_directionsOrder;
	uint32_t						_dirIdx;
	// player pathfinding info
	bool							_findPlayer;
	std::deque<PathNode>			_path;
	// last calculation of path
	std::chrono::milliseconds		_lastFindMs;

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
