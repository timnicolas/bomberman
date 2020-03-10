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
	};
}

class AEntity {
private:
	AEntity();

public:
	// Members
	bool			active;
	Category::Enum	category;
	std::string		name;
	Type::Enum		type;
	SceneGame		&game;

	// Constructors
	explicit AEntity(SceneGame &game);
	virtual ~AEntity();
	AEntity(AEntity const &src);

	// Operators
	AEntity			&operator=(AEntity const &rhs);

	// Methods
	virtual bool		update(float const dTime) = 0;
	virtual bool		draw() = 0;
	virtual bool		isDestructable() = 0;
	virtual bool		blockPropagation() = 0;
	virtual glm::vec2	getPos() = 0;
};

#endif  // ENTITY_HPP_
