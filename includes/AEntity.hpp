#ifndef ENTITY_HPP_
#define ENTITY_HPP_

#include <iostream>
#include <stdexcept>
#include <chrono>
#include <glm/glm.hpp>
#include "glm/gtx/string_cast.hpp"

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
public:
	// Members
	bool			active;
	glm::vec2		pos;
	Category::Enum	category;
	std::string		name;
	Type::Enum		type;

	// Constructors
	AEntity();
	virtual ~AEntity();
	AEntity(AEntity const &src);

	// Operators
	AEntity			&operator=(AEntity const &rhs);

	// Methods
	virtual bool	update(std::chrono::milliseconds d_time) = 0;
	virtual bool	draw() = 0;
	virtual bool	isDestructable() = 0;
	virtual bool	blockPropagation() = 0;
};

#endif  // ENTITY_HPP_
