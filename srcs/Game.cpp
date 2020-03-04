#include <unistd.h>
#include <stdlib.h>
#include <time.h>
// #include <bits/stdc++.h>

#include "Game.hpp"
#include "bomberman.hpp"

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
		player = rhs.player;
		enemies = rhs.enemies;
		bombs = rhs.bombs;
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
	_loadLevel(1);
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
	for (auto &&enemy : enemies) {
		if (!enemy->update(getMs() - last_loop_ms))
			return false;
	}
	for (auto &&bomb : bombs) {
		if (!bomb->update(getMs() - last_loop_ms))
			return false;
	}
	player->update(getMs() - last_loop_ms);

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
	for (auto &&enemy : enemies) {
		if (!enemy->draw())
			return false;
	}
	for (auto &&bomb : bombs) {
		if (!bomb->draw())
			return false;
	}
	player->draw();

	return true;
}

bool	Game::_loadLevel(uint8_t level) {
	SettingsJson	lvl;
	std::string		filename = "bomberman-assets/maps/level"+std::to_string(level)+".json";

	// File json definition:

	lvl.name("level"+std::to_string(level)).description("Level map");
	lvl.add<std::string>("level"+std::to_string(level)+"Filename", filename);
	lvl.add<std::string>("name");
	lvl.add<uint64_t>("height", 0).setMin(0);
	lvl.add<uint64_t>("width", 0).setMin(0);

	lvl.add<SettingsJson>("objects");
		lvl.j("objects").add<std::string>("wall", "w");
		lvl.j("objects").add<std::string>("empty", " ");
		lvl.j("objects").add<std::string>("player", "p");
		lvl.j("objects").add<std::string>("tree", "t");
		lvl.j("objects").add<std::string>("box", "b");
		lvl.j("objects").add<std::string>("end", "e");
		lvl.j("objects").add<std::string>("bomb", "x");

	SettingsJson * mapPattern = new SettingsJson();
	mapPattern->add<std::string>("line", "");
	lvl.addList<SettingsJson>("map", mapPattern);

	SettingsJson * bonusPattern = new SettingsJson();
	bonusPattern->add<SettingsJson>("pos");
	bonusPattern->j("pos").add<uint64_t>("x", 0);
	bonusPattern->j("pos").add<uint64_t>("y", 0);
	bonusPattern->add<std::string>("type", "");
	lvl.addList<SettingsJson>("bonus", bonusPattern);

	try {
		if (lvl.loadFile(filename) == false) {
			// warning when loading settings
			return false;
		}
	} catch(SettingsJson::SettingsException const & e) {
		logErr(e.what());
		return false;
	}

	// Getting json info

	size = {lvl.u("width"), lvl.u("height")};
	board = std::vector< std::vector<std::vector<AEntity*>> >(size.x,
			std::vector< std::vector<AEntity*> >(size.y,
			std::vector< AEntity* >()));

	if (lvl.lj("map").list.size() != size.y)
		throw GameException("Map height error");
	for (uint32_t j = 0; j < size.y; j++) {
		std::string line = lvl.lj("map").list[j]->s("line");
		if (line.length() != size.x)
			throw GameException(("Map width error on line "+std::to_string(j)).c_str());
		for (uint32_t i = 0; i < size.x; i++) {
			for (auto &&entitYCall : _entitiesCall) {
				if (line[i] == lvl.j("objects").s(entitYCall.first)[0]) {
					board[i][j].push_back(_entitiesCall[entitYCall.first]);
				}
			}
		}
	}

	return true;
}

// -- Exceptions errors --------------------------------------------------------

Game::GameException::GameException()
: std::runtime_error("Game Exception") {}

Game::GameException::GameException(const char* what_arg)
: std::runtime_error(std::string(std::string("GameError: ") + what_arg).c_str()) {}
