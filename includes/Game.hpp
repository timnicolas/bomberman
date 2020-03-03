#ifndef GAME_HPP_
#define GAME_HPP_

#include <iostream>
#include <stdexcept>
#include <vector>
#include <map>
#include <glm/glm.hpp>
#include "glm/gtx/string_cast.hpp"

#include "AEntity.hpp"
#include "ACharacter.hpp"

namespace State {
	enum Enum {
		PLAY,
		PAUSE,
		GAME_OVER,
	};
}

class Game {
private:
	// Members
	static std::map<std::string, AEntity *> _entitiesCall;

	// Methods
	bool	_update(std::chrono::milliseconds last_loop_ms);
	bool	_draw();
	bool	_loadLevel(uint8_t level);

public:
	// Members
	// TODO(ebaudet): change board type to std::map<glm::ivec2 pos, std::vector<AEntity*>>
	// AEntity***board;
	std::vector< std::vector< std::vector< AEntity* > > > board;
	// std::map<glm::ivec2, std::vector<AEntity*>> board;
	std::vector<ACharacter *>	characters;
	glm::uvec2					size;
	uint8_t						level;
	State::Enum					state;
	std::chrono::milliseconds	time;

	// Constructors
	Game();
	~Game();
	Game(Game const &src);

	// Operators
	Game &operator=(Game const &rhs);
	friend std::ostream& operator<<(std::ostream& os, const Game& my_class);

	// Methods
	std::string		print() const;
	bool			init();
	bool			run();

	// Exceptions
	class GameException : public std::runtime_error {
	public:
		GameException();
		explicit GameException(const char* what_arg);
	};
};

#endif  // GAME_HPP_
