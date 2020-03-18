#ifndef BONUS_HPP_
#define BONUS_HPP_

#include <iostream>
#include <stdexcept>
#include "AObject.hpp"
#include "SceneGame.hpp"

namespace BonusType {
	enum Enum {
		LIFE,
		BOMBS,
		FLAMES,
		SPEED,
		WALLPASS,
		DETONATOR,
		BOMBPASS,
		FLAMPASS,
		SHIELD,
		NB_BONUS,  // need to stay at last position
	};
}  // namespace BonusType

class Bonus : public AObject {
private:
	Bonus();
	// Member
	BonusType::Enum		_typeBonus;

public:
	// Constructors
	explicit Bonus(SceneGame &game);
	~Bonus();
	Bonus(Bonus const &src);

	// Operators
	Bonus &operator=(Bonus const &rhs);

	// Methods
	bool			update(float const dTime);
	bool			postUpdate();
	bool			draw(Gui &gui);
	static Bonus*	generateBonus(SceneGame &game, float rate = 0.1f);

	// Exceptions
	class BonusException : public std::runtime_error {
	public:
		BonusException();
		explicit BonusException(const char* whatArg);
	};
};

#endif  // BONUS_HPP_
