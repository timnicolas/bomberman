#ifndef ENTITY_HPP_
#define ENTITY_HPP_

#include <iostream>
#include <stdexcept>
#include <chrono>
#include "useGlm.hpp"
#include "Logging.hpp"
class SceneGame;

namespace Category {
	enum Enum {
		STATIC,
		MOBILE,
	};
}
namespace Type {
	enum Enum {
		PLAYER,
		ENEMY,
		BOMB,
		WALL,
		END,
		FLAG,
		FIRE,
	};
}  // namespace Type

class AEntity {
private:
	AEntity();

public:
	// Members
	bool			active;
	bool			alive;
	Category::Enum	category;
	std::string		name;
	Type::Enum		type;
	SceneGame		&game;
	bool			destructible;
	bool			blockPropagation;

	// Constructors
	explicit AEntity(SceneGame &game);
	virtual ~AEntity();
	AEntity(const AEntity &src);

	// Operators
	AEntity			&operator=(const AEntity &rhs);

	// Methods
	virtual bool		update(const float dTime) = 0;
	virtual bool		draw() = 0;
	virtual glm::vec2	getPos() = 0;
	virtual void		takeDamage(const int damage) = 0;
};

#endif  // ENTITY_HPP_
