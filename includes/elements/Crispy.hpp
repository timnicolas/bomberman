#ifndef CRISPY_HPP_
#define CRISPY_HPP_

#include <iostream>
#include <stdexcept>
#include "AObject.hpp"

class Crispy : public AObject {
private:
	Crispy();
	// Members

public:
	// Constructors
	explicit Crispy(SceneGame &game);
	~Crispy();
	Crispy(Crispy const &src);

	// Operators
	Crispy &operator=(Crispy const &rhs);

	// Methods
	bool	update(float const dTime);
	bool	postUpdate();
	bool	draw(Gui &gui);

	// Exceptions
	class CrispyException : public std::runtime_error {
	public:
		CrispyException();
		explicit CrispyException(const char* whatArg);
	};
};

#endif  // CRISPY_HPP_
