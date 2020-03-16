#pragma once

#include <iostream>
#include <stdexcept>
#include "ACharacter.hpp"
#include "SceneGame.hpp"

/**
 * @brief This is the base enemy object
 */
class AEnemy : public ACharacter {
private:
	AEnemy();

protected:
	virtual bool	_update(float const dTime) = 0;
	virtual bool	_postUpdate() = 0;
	virtual bool	_draw(Gui &gui) = 0;

	/* movements functions */
	bool			_movePatternBasic(float const dTime, std::vector<Direction::Enum> directionOrder, uint32_t & dirIdx);

public:
	// Members
	int			bombs;

	// Constructors
	explicit AEnemy(SceneGame &game);
	~AEnemy();
	AEnemy(AEnemy const &src);

	// Operators
	AEnemy &operator=(AEnemy const &rhs);

	// Methods
	bool			update(float const dTime);
	bool			postUpdate();
	bool			draw(Gui &gui);

	// Exceptions
	class EnemyException : public std::runtime_error {
	public:
		EnemyException();
		explicit EnemyException(const char* whatArg);
	};
};
