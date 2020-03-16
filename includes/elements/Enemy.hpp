#pragma once

#include <iostream>
#include <stdexcept>
#include <vector>
#include "AEnemy.hpp"
#include "SceneGame.hpp"

/**
 * @brief This is the enemy object
 */
class Enemy : public AEnemy {
private:
	Enemy();
	std::vector<Direction::Enum>	_directionsOrder;
	uint32_t						_dirIdx;

protected:
	virtual bool	_update(float const dTime);
	virtual bool	_postUpdate();

public:
	// Constructors
	explicit Enemy(SceneGame &game);
	~Enemy();
	Enemy(Enemy const &src);

	// Operators
	Enemy &operator=(Enemy const &rhs);

	// Methods
	static Enemy *	generateEnemy(SceneGame &game, float rate);
};
