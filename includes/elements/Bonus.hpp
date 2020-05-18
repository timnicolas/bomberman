#ifndef BONUS_HPP_
#define BONUS_HPP_

#include <iostream>
#include <map>
#include <unordered_map>
#include <stdexcept>
#include "AObject.hpp"
#include "SceneGame.hpp"
#include "TextureManager.hpp"

#define DETONATOR_DESC "Bonus Detonator: Explode bombs when you want."

namespace BonusType {
	/**
	 * @brief All types of bonus
	 */
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
		TIME,
		POINTS,
		NB_BONUS,  // need to stay at last position
	};
}  // namespace BonusType

/**
 * @brief Bonus object
 */
class Bonus : public AObject {
private:
	Bonus();
	// Member
	BonusType::Enum		_typeBonus;  /**< Bonus type */
	static std::map<BonusType::Enum, Block::Enum>	_textures;  /**< Bonus texture */
	int					_toDraw;  /**< To draw */
	float				_indestructible;
	// Methods
	BonusType::Enum		_pickBonus();

public:
	static std::unordered_map<std::string, BonusType::Enum>	bonus;  /**< All bonus (link btw str & enum) */
	static std::unordered_map<BonusType::Enum, std::string>	description;  /**< All bonus description */
	static std::unordered_map<BonusType::Enum, std::string>	bonusTextures;  /**< All bonus textures */
	// Constructors
	explicit Bonus(SceneGame &game);
	~Bonus();
	Bonus(Bonus const &src);

	// Operators
	Bonus &operator=(Bonus const &rhs);

	// Methods
	bool				init();
	bool				update();
	bool				postUpdate();
	bool				draw(Gui &gui);
	static std::string	getDescription(BonusType::Enum type);
	static Bonus*		generateBonus(SceneGame &game, float rate = 0.1f);

	// Exceptions
	/**
	 * @brief Bonus Exception
	 */
	class BonusException : public std::runtime_error {
	public:
		BonusException();
		/**
		 * @brief Construct a new Spawner Exception object
		 *
		 * @param whatArg Error message
		 */
		explicit BonusException(const char* whatArg);
	};
};

#endif  // BONUS_HPP_
