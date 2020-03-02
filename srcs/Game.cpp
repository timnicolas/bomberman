#include <unistd.h>
#include <stdlib.h>
#include <time.h>

#include "Game.hpp"
#include "bomberman.hpp"

// -- Constructors -------------------------------------------------------------

Game::Game() {
	board = {{nullptr}};
	// TODO(ebaudet): init members
}

Game::~Game() {
}

Game::Game(Game const &src) {
	*this = src;
}

// -- Operators ----------------------------------------------------------------

Game &Game::operator=(Game const &rhs) {
	if ( this != &rhs ) {
		board = rhs.board;
		characters = rhs.characters;
		size = rhs.size;
		level = rhs.level;
		state = rhs.state;
		time = rhs.time;
	}
	return *this;
}

std::ostream &	operator<<(std::ostream & os, const Game& my_class) {
	os << my_class.print();
	return (os);
}

// -- Methods ------------------------------------------------------------------

/**
 * print params.
 */
std::string		Game::print() const {
	std::string		str;
	str = "Game info: [" + std::to_string(size.x) + ", "
	+ std::to_string(size.y) + "]";

	return str;
}

/**
 * init game method.
 */
bool			Game::init() {
	return true;
}

/**
 * init game method.
 */
bool			Game::run() {
	float						loopTime = 1000 / 60;  // TODO(ebaudet): fps
	std::chrono::milliseconds	time_start;
	std::chrono::milliseconds	last_loop_ms = getMs();

	while (true) {
		time_start = getMs();

		// TODO(ebaudet): input handler here

		_update(last_loop_ms);
		_draw();

		// fps
		std::chrono::milliseconds time_loop = getMs() - time_start;
		if (time_loop.count() > loopTime) {
			#if DEBUG_FPS_LOW == true
				if (!firstLoop)
					logDebug("update loop slow -> " << time_loop.count() << "ms / " << loopTime << "ms (" << FPS << "fps)");
			#endif
		}
		else {
			usleep((loopTime - time_loop.count()) * 1000);
		}
		#if DEBUG_FPS_LOW == true
			firstLoop = false;
		#endif
		last_loop_ms = getMs();
	}
	return true;
}

// -- Private Methods ----------------------------------------------------------

bool	Game::_update(std::chrono::milliseconds last_loop_ms) {
	for (int j = 0; j < size.y; j++) {
		for (int i = 0; i < size.x; i++) {
			if (board[j][i] != nullptr) {
				if (!board[j][i]->update(getMs() - last_loop_ms))
					return false;
			}
		}
	}
	for (auto &&charater : characters) {
		if (!charater->update(getMs() - last_loop_ms))
			return false;
	}

	return true;
}

bool	Game::_draw() {
	for (int j = 0; j < size.y; j++) {
		for (int i = 0; i < size.x; i++) {
			if (board[j][i] != nullptr) {
				if (!board[j][i]->draw())
					return false;
			}
		}
	}
	for (auto &&charater : characters) {
		if (!charater->draw())
			return false;
	}

	return true;
}

void	Game::_loadLevel(uint8_t level) {
	// TODO(ebaudet): load level json.
	(void)level;
}

// -- Exceptions errors --------------------------------------------------------

Game::GameException::GameException()
: std::runtime_error("Game Exception") {}

Game::GameException::GameException(const char* what_arg)
: std::runtime_error(std::string(std::string("GameError: ") + what_arg).c_str()) {}
