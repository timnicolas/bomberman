#ifndef FLAG_HPP_
#define FLAG_HPP_

#include <iostream>
#include <stdexcept>
#include "AObject.hpp"

class Flag : public AObject {
private:
	// Members

public:
	// Constructors
	Flag();
	~Flag();
	Flag(Flag const &src);

	// Operators
	Flag &operator=(Flag const &rhs);

	// Methods
	bool	update(std::chrono::milliseconds d_time);
	bool	draw();
	bool	isDestructable();
	bool	blockPropagation();

	// Exceptions
	class FlagException : public std::runtime_error {
	public:
		FlagException();
		explicit FlagException(const char* what_arg);
	};
};

#endif  // FLAG_HPP_
