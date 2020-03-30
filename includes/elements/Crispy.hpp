#ifndef CRISPY_HPP_
#define CRISPY_HPP_

#include <iostream>
#include <stdexcept>
#include "AObject.hpp"

class Crispy : public AObject {
private:
	Crispy();

public:
	// Constructors
	explicit Crispy(SceneGame &game);
	~Crispy();
	Crispy(Crispy const &src);

	// Operators
	Crispy &operator=(Crispy const &rhs);

	// Methods
	bool	update();
	bool	postUpdate();
	bool	draw(Gui &gui);

	static Crispy *	generateCrispy(SceneGame &game, uint32_t genWallPercent);

	// Exceptions
	class CrispyException : public std::runtime_error {
	public:
		CrispyException();
		explicit CrispyException(const char* whatArg);
	};
};

#endif  // CRISPY_HPP_
