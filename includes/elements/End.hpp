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
	Block::Enum	_texture;
	bool		_open;
	Model		*_model2;

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
	class EndException : public std::runtime_error {
	public:
		EndException();
		explicit EndException(const char* whatArg);
	};
};

#endif  // END_HPP_
