#ifndef END_HPP_
#define END_HPP_

#include <iostream>
#include <unordered_set>
#include <stdexcept>
#include "AObject.hpp"

#define END_STR "end"
#define OPEN_PORTAL_TEXTURE "bomberman-assets/3dModels/exit/textures/open_portal_shield.png"
#define CLOSE_PORTAL_TEXTURE "bomberman-assets/3dModels/exit/textures/close_portal_shield.png"

/**
 * @brief This is the end object
 */
class End : public AObject {
private:
	End();
	// Members
	Block::Enum	_texture;  /**< Object texture */
	bool		_open;  /**< Is open ? (when all flags are destroyed) */
	Model		*_model2;  /**< second model, used for the transparent tube */

	// Methods
	std::unordered_set<AEntity *>	_getCollision();

public:
	// Constructors
	explicit End(SceneGame &game);
	~End();
	End(End const &src);

	// Operators
	End &operator=(End const &rhs);

	// Methods
	bool	init();
	bool	update();
	bool	draw(Gui &gui);

	// Exceptions
	/**
	 * @brief End Exception
	 */
	class EndException : public std::runtime_error {
	public:
		EndException();
		/**
		 * @brief Construct a new Spawner Exception object
		 *
		 * @param whatArg Error message
		 */
		explicit EndException(const char* whatArg);
	};
};

#endif  // END_HPP_
