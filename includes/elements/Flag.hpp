#ifndef FLAG_HPP_
#define FLAG_HPP_

#include <iostream>
#include <stdexcept>
#include "AObject.hpp"

/**
 * @brief This is the flag object
 */
class Flag : public AObject {
private:
	Flag();
	// Members

public:
	// Constructors
	explicit Flag(SceneGame &game);
	~Flag();
	Flag(Flag const &src);

	// Operators
	Flag &operator=(Flag const &rhs);

	// Methods
	bool	update(std::chrono::milliseconds dTime);
	bool	draw();
	bool	isDestructable();
	bool	blockPropagation();

	// Exceptions
	class FlagException : public std::runtime_error {
	public:
		FlagException();
		explicit FlagException(const char* whatArg);
	};
};

#endif  // FLAG_HPP_
