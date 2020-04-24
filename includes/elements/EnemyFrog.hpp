#pragma once

#include <iostream>
#include <stdexcept>
#include <vector>
#include "AEnemy.hpp"
#include "SceneGame.hpp"

#define ENEMY_FROG_STR "enemyFrog"
#define WAIT_JUMP_TIME_MS		1000
#define LONG_WAIT_JUMP_TIME_MS	5000
#define CHANCE_LONG_WAIT		5  // one chance on 5 to have a long wait

/**
 * @brief This is an enemy object
 *
 * EnemyFrog can jump across one bloc
 */
class EnemyFrog : public AEnemy {
private:
	EnemyFrog();

	glm::ivec2		_jumpGoal;
	uint64_t		_nextJumpTime;

protected:
	virtual bool	_update();
	virtual bool	_postUpdate();
	virtual bool	_draw(Gui &gui);
	void			_findJumpGoal();
	void			_jumpCrossable();

public:
	// Constructors
	explicit EnemyFrog(SceneGame &game);
	~EnemyFrog();
	EnemyFrog(EnemyFrog const &src);

	// Operators
	EnemyFrog &operator=(EnemyFrog const &rhs);

	// Methods
	virtual void	resetCrossable();
};
