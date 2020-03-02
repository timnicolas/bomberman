#ifndef ENTITY_HPP_
#define ENTITY_HPP_

#include <iostream>
#include <stdexcept>
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
		IA,
		BOMB,
		WALL,
	};
}

class AEntity {
public:
	// Members
	glm::vec2		pos;
	Category::Enum	category;
	std::string		name;
	Type::Enum		type;
	bool			active;

	// Constructors
	AEntity();
	virtual ~AEntity();
	AEntity(AEntity const &src);

	// Operators
	AEntity			&operator=(AEntity const &rhs);

	// Methods
	virtual bool	update(float d_time) = 0;
	virtual bool	draw() = 0;
};

#endif  // ENTITY_HPP_
