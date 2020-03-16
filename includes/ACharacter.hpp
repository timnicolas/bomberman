#ifndef ACHARACTER_HPP_
#define ACHARACTER_HPP_

#include <iostream>
#include <stdexcept>
#include <unordered_set>
#include "useGlm.hpp"
#include "AEntity.hpp"

// class SceneGame;
namespace Direction {
	enum Enum {
		UP = 0,
		RIGHT,
		DOWN,
		LEFT,
		NB_DIRECTIONS  // keep it at last position
	};
}
/**
 * @brief This is the base for Charactere (Player, AI, ...)
 */
class ACharacter : public AEntity {
private:
	ACharacter();

protected:
	// Members
	std::unordered_set<AEntity *>	_noCollisionObjects;

	// Methods
	void	_clearCollisionObjects(std::unordered_set<AEntity *> collisions);
	bool	_canMove(std::unordered_set<AEntity *> collisions);
	glm::vec3	_moveTo(Direction::Enum direction, float const dTime);

public:
	// Members
	int			lives;
	float		speed;

	// Constructors
	explicit ACharacter(SceneGame &game);
	virtual ~ACharacter();
	ACharacter(ACharacter const &src);

	// Operators
	ACharacter &operator=(ACharacter const &rhs);

	// Methods
	virtual bool					update(float const dTime) = 0;
	virtual bool					draw(Gui &gui) = 0;
	bool							isAlive();
	glm::vec3						getPos();
	ACharacter						*setPosition(glm::vec3 pos);
	bool							takeDamage(const int damage);
	std::unordered_set<AEntity *>	getCollision(glm::vec3 pos, float offset = 0.05f);
	bool							clearNoCollisionObjects(AEntity *entity);
	bool							hasCollision(glm::vec3 pos, float offset = 0.2f);


	// Exceptions
	class ACharacterException : public std::runtime_error {
	public:
		ACharacterException();
		explicit ACharacterException(const char* whatArg);
	};
};

#endif  // ACHARACTER_HPP_
