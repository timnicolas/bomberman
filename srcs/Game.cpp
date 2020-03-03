#include <unistd.h>
#include <stdlib.h>
#include <time.h>
// #include <bits/stdc++.h>

#include "Game.hpp"
#include "bomberman.hpp"
#include "Player.hpp"
#include "Bomb.hpp"

// -- Static members initialisation --------------------------------------------

std::map<std::string, AEntity *> Game::_entitiesCall = {
	{"player", new Player()},
	{"bomb", new Bomb()},
};

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
	float						loopTime = 1000 / s.j("screen").u("fps");
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
	for (auto &&board_it1 : board) {
		for (auto &&board_it1 : board_it1) {
			for (AEntity *board_it2 : board_it1) {
				if (!board_it2->update(getMs() - last_loop_ms))
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
	for (auto &&board_it1 : board) {
		for (auto &&board_it1 : board_it1) {
			for (AEntity *board_it2 : board_it1) {
				if (!board_it2->draw())
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

bool	Game::_loadLevel(uint8_t level) {
	SettingsJson	lvl;
	std::string		filename = "bomberman-assets/maps/level"+std::to_string(level)+".json";

	lvl.name("level"+std::to_string(level)).description("Level map");
	lvl.add<std::string>("level"+std::to_string(level)+"Filename", filename);
	lvl.add<std::string>("name");
	lvl.add<uint64_t>("height", 0);
	lvl.add<uint64_t>("width", 0);
	lvl.j("objects").add<char>("wall", 'w');
	lvl.j("objects").add<char>("empty", ' ');
	lvl.j("objects").add<char>("player", 'p');
	lvl.j("objects").add<char>("tree", 't');
	lvl.j("objects").add<char>("box", 'b');
	lvl.j("objects").add<char>("end", 'e');

	// lvl.j("map").add<list>(null, nullptr);
	lvl.j("map").add<std::string>("0", "");
	lvl.j("map").add<std::string>("1", "");
	lvl.j("map").add<std::string>("2", "");
	lvl.j("map").add<std::string>("3", "");
	lvl.j("map").add<std::string>("4", "");
	lvl.j("map").add<std::string>("5", "");
	lvl.j("map").add<std::string>("6", "");
	lvl.j("map").add<std::string>("7", "");
	lvl.j("map").add<std::string>("8", "");

	// lvl.j("bonus").get<list>(nullptr).j("pos").add<uint64_t>("x", 0);
	// lvl.j("bonus").get<list>(nullptr).j("pos").add<uint64_t>("y", 0);
	// lvl.j("bonus").get<list>(nullptr).add<std::string>("type");
	lvl.j("bonus").j("0").j("pos").add<uint64_t>("x", 0);
	lvl.j("bonus").j("0").j("pos").add<uint64_t>("y", 0);
	lvl.j("bonus").j("0").add<std::string>("type");

	lvl.j("bonus").j("1").j("pos").add<uint64_t>("x", 0);
	lvl.j("bonus").j("1").j("pos").add<uint64_t>("y", 0);
	lvl.j("bonus").j("1").add<std::string>("type");

	try {
		if (lvl.loadFile(filename) == false) {
			// warning when loading settings
			return false;
		}
	} catch(SettingsJson::SettingsException const & e) {
		logErr(e.what());
		return false;
	}

	size = {lvl.u("width"), lvl.u("width")};
	board = std::vector< std::vector<std::vector<AEntity*>> >(size.x,
			std::vector< std::vector<AEntity*> >(size.y,
			std::vector< AEntity* >()));

	for (uint32_t j = 0; j < size.y; j++) {
		std::string line = lvl.j("map").s(std::to_string(j));
		if (line.length() != size.x)
			throw GameException(("Map length error on line "+std::to_string(j)).c_str());
		for (uint32_t i = 0; i < size.x; i++) {
			line[i];
			if (_entitiesCall.find("player") != _entitiesCall.end())
				board[i][j].push_back(_entitiesCall["player"]);
		}
	}

	return true;
}

// -- Exceptions errors --------------------------------------------------------

Game::GameException::GameException()
: std::runtime_error("Game Exception") {}

Game::GameException::GameException(const char* what_arg)
: std::runtime_error(std::string(std::string("GameError: ") + what_arg).c_str()) {}
