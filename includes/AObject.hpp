#ifndef AOBJECT_HPP_
#define AOBJECT_HPP_

#include <iostream>
#include <stdexcept>
#include "AEntity.hpp"

class AObject : public AEntity {
public:
	// Constructors
	AObject();
	virtual ~AObject();
	AObject(AObject const &src);

	// Operators
	AObject &operator=(AObject const &rhs);

	// Methods
	virtual bool	update(float d_time) = 0;
	virtual bool	draw() = 0;

	// Exceptions
	class AObjectException : public std::runtime_error {
	public:
		AObjectException();
		explicit AObjectException(const char* what_arg);
	};
};

#endif  // AOBJECT_HPP_
