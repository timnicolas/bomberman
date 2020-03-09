#ifndef END_HPP_
#define END_HPP_

#include <iostream>
#include <stdexcept>
#include "AObject.hpp"

class End : public AObject {
private:
	End();
	// Members

public:
	// Constructors
	explicit End(SceneGame &game);
	~End();
	End(End const &src);

	// Operators
	End &operator=(End const &rhs);

	// Methods
	bool	update(std::chrono::milliseconds dTime);
	bool	draw();
	bool	isDestructable();
	bool	blockPropagation();

	// Exceptions
	class EndException : public std::runtime_error {
	public:
		EndException();
		explicit EndException(const char* whatArg);
	};
};

#endif  // END_HPP_
