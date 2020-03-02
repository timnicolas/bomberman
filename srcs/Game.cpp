#include "Game.hpp"

// -- Constructors -------------------------------------------------------------

Game::Game() {
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

// TODO(ebaudet): put here the methods.

// -- Exceptions errors --------------------------------------------------------

Game::GameException::GameException()
: std::runtime_error("Game Exception") {}

Game::GameException::GameException(const char* what_arg)
: std::runtime_error(std::string(std::string("GameError: ") + what_arg).c_str()) {}
