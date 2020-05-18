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
	bool	update();
	bool	postUpdate();
	bool	draw(Gui &gui);

	// Exceptions
	/**
	 * @brief Flag exception
	 */
	class FlagException : public std::runtime_error {
	public:
		FlagException();
		/**
		 * @brief Construct a new Spawner Exception object
		 *
		 * @param whatArg Error message
		 */
		explicit FlagException(const char* whatArg);
	};
};

#endif  // FLAG_HPP_
