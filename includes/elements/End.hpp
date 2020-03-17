#ifndef END_HPP_
#define END_HPP_

#include <iostream>
#include <unordered_set>
#include <stdexcept>
#include "AObject.hpp"

/**
 * @brief This is the end object
 */
class End : public AObject {
private:
	End();
	// Members
	Block::Enum	_texture;

	// Methods
	std::unordered_set<AEntity *>	_getCollision(float offset);

public:
	// Constructors
	explicit End(SceneGame &game);
	~End();
	End(End const &src);

	// Operators
	End &operator=(End const &rhs);

	// Methods
	bool	update(float const dTime);
	bool	draw(Gui &gui);

	// Exceptions
	class EndException : public std::runtime_error {
	public:
		EndException();
		explicit EndException(const char* whatArg);
	};
};

#endif  // END_HPP_
