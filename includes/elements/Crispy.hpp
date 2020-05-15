#ifndef CRISPY_HPP_
#define CRISPY_HPP_

#include <iostream>
#include <stdexcept>
#include "AObject.hpp"

#define CRISPY_STR "crispy"

class Crispy : public AObject {
private:
	Crispy();

	void	_updateAnimationState();

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
	bool	init();

	static Crispy *	generateCrispy(SceneGame &game, uint32_t genWallPercent);

	// Exceptions
	class CrispyException : public std::runtime_error {
	public:
		CrispyException();
		/**
		 * @brief Construct a new Spawner Exception object
		 *
		 * @param whatArg Error message
		 */
		explicit CrispyException(const char* whatArg);
	};
};

#endif  // CRISPY_HPP_
