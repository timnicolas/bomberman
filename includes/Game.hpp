#ifndef GAME_HPP_
#define GAME_HPP_

#include <iostream>
#include <stdexcept>
#include <vector>
#include <glm/glm.hpp>
#include "glm/gtx/string_cast.hpp"

#include "AEntity.hpp"
#include "ACharacter.hpp"

class Game {
private:
	// Members

public:
	// Members
	AEntity						***board;
	std::vector<ACharacter *>	characters;
	glm::ivec2					size;

	// Constructors
	Game();
	~Game();
	Game(Game const &src);

	// Operators
	Game &operator=(Game const &rhs);
	friend std::ostream& operator<<(std::ostream& os, const Game& my_class);

	// Methods
	std::string		print() const;

	// Exceptions
	class GameException : public std::runtime_error {
	public:
		GameException();
		explicit GameException(const char* what_arg);
	};
};

#endif  // GAME_HPP_
