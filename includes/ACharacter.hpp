#ifndef ACHARACTER_HPP_
#define ACHARACTER_HPP_

#include <iostream>
#include <stdexcept>
#include <unordered_set>
#include "useGlm.hpp"
#include "AEntity.hpp"

#define MIN_SPEED 1.0
#define MAX_SPEED 6.0
#define OFFSET_TURN_CORRECTION 0.85
#define MOVE_STEP 0.05

// class SceneGame;
namespace Direction {
	enum Enum {
		NO_DIRECTION = -1,
		UP = 0,
		RIGHT = 1,
		DOWN = 2,
		LEFT = 3,
		NB_DIRECTIONS  // keep it at last position
	};
}
/**
 * @brief This is the base for Charactere (Player, AI, ...)
 */
class ACharacter : public AEntity {
private:
	ACharacter();
	glm::vec3	_miniMove(glm::vec3 movement);

protected:
	// Methods
	std::unordered_set<AEntity *>	_getAllBlockableEntity(glm::vec3 dest) const;
	bool		_canWalkOnBlock(glm::ivec2 pos) const;
	bool		_canWalkOnEntity(AEntity * entity) const;
	bool		_canMoveOnFromTo(glm::vec3 from, glm::vec3 to) const;
	bool		_canMoveOn(glm::vec3 dest) const;
	virtual glm::vec3	_moveTo(Direction::Enum direction, float const offset = OFFSET_TURN_CORRECTION);
	virtual glm::vec3	_moveTo(glm::vec3 direction, float const offset = OFFSET_TURN_CORRECTION);
	std::vector<glm::ivec2>	_getAllPositions(glm::vec3 dest, glm::vec3 size) const;


public:
	// Members
	int			lives;
	float		speed;
	glm::vec3	front;
	std::vector<Type::Enum>	crossableTypes;  // all types that the Character can cross

	// Constructors
	explicit ACharacter(SceneGame &game);
	virtual ~ACharacter();
	ACharacter(ACharacter const &src);

	// Operators
	ACharacter &operator=(ACharacter const &rhs);

	// Methods
	virtual void					resetCrossable();
	virtual bool					update() = 0;
	virtual bool					draw(Gui &gui) = 0;
	virtual std::vector< std::vector< std::vector<AEntity *> > > const &	getBoard() const;
	virtual std::vector< std::vector< std::vector<AEntity *> > > &			getBoard();
	bool							isAlive();
	glm::vec3						getPos() const;
	glm::ivec2						getIntPos() const;
	ACharacter						*setPosition(glm::vec3 pos);
	bool							takeDamage(const int damage);
	virtual std::unordered_set<AEntity *>	getCollision(glm::vec3 dest) const;
	bool							hasCollision(glm::vec3 atPosition, glm::vec3 atSize = glm::vec3(1, 1, 1));
	bool							tp(glm::vec3 tpPos);

	// Exceptions
	class ACharacterException : public std::runtime_error {
	public:
		ACharacterException();
		explicit ACharacterException(const char* whatArg);
	};
};

#endif  // ACHARACTER_HPP_
