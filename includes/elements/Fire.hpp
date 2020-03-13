#ifndef FIRE_HPP_
#define FIRE_HPP_

#include <iostream>
#include <stdexcept>
#include "AObject.hpp"

class Fire : public AObject {
private:
	Fire();
	// Members

public:
	// Constructors
	explicit Fire(SceneGame &game);
	~Fire();
	Fire(Fire const &src);

	// Operators
	Fire &operator=(Fire const &rhs);

	// Methods
	bool	update(float const dTime);
	bool	postUpdate();
	bool	draw(Gui &gui);

	// Exceptions
	class FireException : public std::runtime_error {
	public:
		FireException();
		explicit FireException(const char* whatArg);
	};
};

#endif  // FIRE_HPP_
