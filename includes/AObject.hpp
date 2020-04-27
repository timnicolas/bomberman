#ifndef AOBJECT_HPP_
#define AOBJECT_HPP_

#include <iostream>
#include <stdexcept>
#include "AEntity.hpp"

/**
 * @brief This is the base class for objects (Bomb, wall, ...)
 */
class AObject : public AEntity {
private:
	AObject();

protected:
	std::string		_soundOfDeath;

public:
	// Constructors
	explicit AObject(SceneGame &game);
	virtual ~AObject();
	AObject(AObject const &src);

	// Operators
	AObject &operator=(AObject const &rhs);

	// Methods
	virtual bool	update() = 0;
	virtual bool	draw(Gui &gui) = 0;
	void			setPos(glm::vec3 pos = VOID_POS3);
	glm::vec3		getPos() const;
	bool			takeDamage(int damage);
	virtual std::vector< std::vector< std::vector<AEntity *> > > const &	getBoard() const;
	virtual std::vector< std::vector< std::vector<AEntity *> > > &			getBoard();
	virtual bool	init();

	bool			isInFlyBoard;

	// Exceptions
	class AObjectException : public std::runtime_error {
	public:
		AObjectException();
		explicit AObjectException(const char* whatArg);
	};
};

#endif  // AOBJECT_HPP_
