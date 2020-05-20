#ifndef FIRE_HPP_
#define FIRE_HPP_

#include <iostream>
#include <stdexcept>
#include "AObject.hpp"

/**
 * @brief Fire on bombs objects
 */
class Fire : public AObject {
private:
	Fire();

public:
	// Constructors
	explicit Fire(SceneGame &game);
	~Fire();
	Fire(Fire const &src);

	// Operators
	Fire &operator=(Fire const &rhs);

	// Methods
	bool	init();
	bool	update();
	bool	postUpdate();
	bool	draw(Gui &gui);

	// Exceptions
	/**
	 * @brief Fire Exception
	 */
	class FireException : public std::runtime_error {
	public:
		FireException();
		/**
		 * @brief Construct a new Spawner Exception object
		 *
		 * @param whatArg Error message
		 */
		explicit FireException(const char* whatArg);
	};
};

#endif  // FIRE_HPP_
