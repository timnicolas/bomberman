#ifndef FLAG_HPP_
#define FLAG_HPP_

#include <iostream>
#include <stdexcept>
#include "AObject.hpp"

#define FLAG_STR "flag"

/**
 * @brief This is the flag object
 */
class Flag : public AObject {
private:
	Flag();

public:
	// Constructors
	explicit Flag(SceneGame &game);
	~Flag();
	Flag(Flag const &src);

	// Operators
	Flag &operator=(Flag const &rhs);

	// Methods
	bool	init();
	bool	update();
	bool	postUpdate();
	bool	draw(Gui &gui);

	// Exceptions
	class FlagException : public std::runtime_error {
	public:
		FlagException();
		explicit FlagException(const char* whatArg);
	};
};

#endif  // FLAG_HPP_
