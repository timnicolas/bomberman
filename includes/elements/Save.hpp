#ifndef SAVE_HPP_
#define SAVE_HPP_

#include <iostream>
#include <stdexcept>

class Save {
private:
	// Members
	int		_member;

public:
	// Constructors
	Save();
	explicit Save(int member);
	~Save();
	Save(Save const &src);

	// Operators
	Save &operator=(Save const &rhs);
	friend std::ostream& operator<<(std::ostream& os, const Save& my_class);

	// Accessors
	int		getMember() const;
	void	setMember(int member);

	// Methods
	bool	initJson();

	// Exceptions
	class SaveException : public std::runtime_error {
	public:
		SaveException();
		explicit SaveException(const char* what_arg);
	};
};

#endif  // SAVE_HPP_
