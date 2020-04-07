#include <array>
#include <regex>

#include "Save.hpp"
#include "FileUtils.hpp"

#include "ACharacter.hpp"
#include "Player.hpp"

// -- Constructors -------------------------------------------------------------

Save::Save() {
	_instantiate = false;
	_saved = false;
}

Save::~Save() {
	delete _saveJs;
}

Save::Save(Save const &src) {
	_saveJs = new SettingsJson();
	*this = src;
}

// -- get ----------------------------------------------------------------------
/**
 * @brief Return a reference to the singleton Save.
 *
 * @return Save& the reference to the singleton.
 */
Save	&Save::get() {
	static Save	instance;
	return (instance);
}

/**
 * @brief Load filename.
 *
 * @param filename
 * @return Save&
 */
Save	&Save::loadGame(std::string filename) {
	return get()._loadGame(filename);
}

Save	&Save::_loadGame(std::string filename) {
	_instantiate = false;
	_init();
	_saveJs = new SettingsJson();
	if (file::isFile(filename) == false) {
		logErr(filename << " not exist");
		return *this;
	}
	std::regex	regex(_fileNameRegex);
	std::smatch	match;
	std::regex_match(filename, match, regex);
	if (match.size() >= 2) {
		_time = static_cast<time_t>(std::stoi(match[1]));
		if (file::isFile(filename) == false) {
			logInfo(filename << " not exist and will be created.");
			return *this;
		}
	} else {
		logErr("FileNameError: " << filename);
		return *this;
	}
	if (_initJson())
		_instantiate = true;
	_saved = true;
	return *this;
}

Save	&Save::newGame() {
	return get()._newGame();
}
Save	&Save::_newGame() {
	if (isInstantiate())
		deleteTemp();
	_init();
	_saveJs = new SettingsJson();
	_initJson(true);
	_instantiate = true;
	_saved = false;
	return *this;
}

// -- Operators ----------------------------------------------------------------

Save &Save::operator=(Save const &rhs) {
	if ( this != &rhs ) {
		_time = rhs._time;
		*_saveJs = *(rhs._saveJs);
		_fileNameRegex = rhs._fileNameRegex;
	}
	return *this;
}

std::ostream &	operator<<(std::ostream & os, const Save& my_class) {
	os << "Save: " << my_class._saveJs;
	return (os);
}

// -- Getters & Setters --------------------------------------------------------

std::string	Save::getFileName(bool temporary) {
	return get()._getFileName(temporary);
}

std::string	Save::_getFileName(bool temporary) const {
	return s.s("savePath") + std::to_string(_time) + (temporary ? "_temp" : "") + ".save";
}

bool		Save::isInstantiate() {
	return get()._instantiate;
}
bool		Save::isSaved() {
	return get()._saved;
}
// -- Private methods ----------------------------------------------------------

void	Save::_init() {
	_time = std::time(nullptr);
	_fileNameRegex = "^" + Save::_addRegexSlashes(s.s("savePath")) + "(\\d{10,})(_temp)?\\.save$";
}

// -- Methods ------------------------------------------------------------------

std::string	Save::_addRegexSlashes(std::string str) {
	char	specialChar[] = {'\'', '"', '?', '\\', '/', '.'};
	std::string::iterator	it = str.begin();
	while (it != str.end()) {
		if (std::find(specialChar, specialChar+5, *it) < specialChar+5) {
			// insert '\'
			it = str.insert(it, '\\');
			it++;
		}
		it++;
	}
	return str;
}


/**
 * @brief Define Json format to saved file.
 *
 * @return true
 * @return false
 */
bool	Save::_initJson(bool newFile) {
	try {
		std::string		saveName = std::to_string(_time);
		_filename = _getFileName(false);
		_saveJs->name(saveName).description("Save file");
		_saveJs->add<std::string>("Filename", _filename);

		// Save json definition
		_saveJs->add<std::string>("name");
		_saveJs->add<int64_t>("date_creation", _time);
		std::time_t		t = std::time(nullptr);
		_saveJs->add<int64_t>("date_lastmodified", t);

		SettingsJson	*levelPattern = new SettingsJson();
		levelPattern->add<int64_t>("id", 0).setMin(0);
		levelPattern->add<int64_t>("score", 0).setMin(0).setMax(99999999);
		_saveJs->addList<SettingsJson>("levels", levelPattern);

		_saveJs->add<SettingsJson>("state");
			_saveJs->j("state").add<uint64_t>("life", 2).setMin(1).setMax(999);
			_saveJs->j("state").add<uint64_t>("bombs", 1).setMin(1).setMax(999);
			_saveJs->j("state").add<uint64_t>("flame", 2).setMin(1).setMax(999);
			_saveJs->j("state").add<double>("speed", 3.0).setMin(3.0).setMax(MAX_SPEED);
			_saveJs->j("state").add<uint64_t>("wallpass", 0).setMin(0).setMax(1);
			_saveJs->j("state").add<uint64_t>("detonator", 0).setMin(0).setMax(1);
			_saveJs->j("state").add<uint64_t>("bombpass", 0).setMin(0).setMax(1);
			_saveJs->j("state").add<uint64_t>("flampass", 0).setMin(0).setMax(1);

		if (!newFile) {
			if (_saveJs->loadFile(_filename) == false) {
				logErr("Fail loading " << _filename);
				return false;
			}
		}
	} catch (SettingsJson::SettingsException const & e) {
		logErr(e.what());
		return false;
	}
	try {
		_saveJs->saveToFile(_getFileName(true));
	} catch (SettingsJson::SettingsException &e) {
		logErr(e.what());
		return false;
	}

	return true;
}

/**
 * @brief Update Saved File Informations
 *
 * @param game
 * @return true
 * @return false
 */
bool	Save::updateSavedFile(SceneGame &game, bool succeedLevel) {
	return get()._updateSavedFile(game, succeedLevel);
}
bool	Save::_updateSavedFile(SceneGame &game, bool succeedLevel) {
	logInfo("Save: LevelID: " << game.level);
	logInfo("Save: Score: " << game.score);

	_saved = false;
	std::time_t		t = std::time(nullptr);
	_saveJs->i("date_lastmodified") = t;
	_saveJs->j("state").u("life") = game.player->lives;
	_saveJs->j("state").u("bombs") = game.player->totalBombs;
	_saveJs->j("state").u("flame") = game.player->bombProgation;
	_saveJs->j("state").d("speed") = game.player->speed;
	_saveJs->j("state").u("wallpass") = game.player->passWall == true ? 1 : 0;
	_saveJs->j("state").u("detonator") = game.player->detonator == true ? 1 : 0;
	_saveJs->j("state").u("bombpass") = game.player->passBomb == true ? 1 : 0;
	_saveJs->j("state").u("flampass") = game.player->passFire == true ? 1 : 0;

	if (succeedLevel) {
		bool	levelAlreadyDone = false;
		for (SettingsJson *level : _saveJs->lj("levels").list) {
			if (level->i("id") != game.level)
				continue;
			else {
				if (level->i("score") < game.score.getScore())
					level->i("score") = game.score.getScore();
				levelAlreadyDone = true;
				break;
			}
		}
		if (!levelAlreadyDone) {
			SettingsJson	*levelPattern = new SettingsJson();
			levelPattern->add<int64_t>("id", 0).setMin(0);
			levelPattern->add<int64_t>("score", 0).setMin(0).setMax(99999999);
			levelPattern->i("id") = game.level;
			levelPattern->i("score") = game.score.getScore();

			_saveJs->lj("levels").add(levelPattern);
		}
	}

	return true;
}

/**
 * @brief Load information from saved files.
 *
 * @param game
 * @return true
 * @return false
 */
bool	Save::loadStatesSaved(SceneGame &game) {
	return get()._loadStatesSaved(game);
}
bool	Save::_loadStatesSaved(SceneGame &game) {
	game.player->lives = _saveJs->j("state").u("life");
	game.player->totalBombs = _saveJs->j("state").u("bombs");
	game.player->bombProgation = _saveJs->j("state").u("flame");
	game.player->speed = _saveJs->j("state").d("speed");
	game.player->passWall = _saveJs->j("state").u("wallpass") == 0 ? false : true;
	game.player->detonator = _saveJs->j("state").u("detonator") == 0 ? false : true;
	game.player->passBomb = _saveJs->j("state").u("bombpass") == 0 ? false : true;
	game.player->passFire = _saveJs->j("state").u("flampass") == 0 ? false : true;

	return true;
}

/**
 * @brief Check if the Level <levelId> has already been done.
 *
 * @param game
 * @return true
 * @return false
 */
bool	Save::isLevelDone(int32_t levelId) {
	return get()._isLevelDone(levelId);
}
bool	Save::_isLevelDone(int32_t levelId) {
	for (SettingsJson *level : _saveJs->lj("levels").list) {
		if (level->i("id") == levelId) {
			return true;
		}
	}
	return false;
}

/**
 * @brief Get score for given levelId.
 *
 * @param levelId
 * @return int score. 0 is returned if not score for the given level.
 */
int		Save::getLevelScore(int32_t levelId) {
	return get()._getLevelScore(levelId);
}
int		Save::_getLevelScore(int32_t levelId) {
	for (SettingsJson *level : _saveJs->lj("levels").list) {
		if (level->i("id") == levelId) {
			return level->i("score");
		}
	}
	return 0;
}

/**
 * @brief Save method to file.
 *
 * @param temporary
 * @return true
 * @return false
 */
bool	Save::save(bool temporary) {
	return get()._save(temporary);
}
bool	Save::_save(bool temporary) {
	try {
		_saveJs->saveToFile(_getFileName(temporary));
	} catch (SettingsJson::SettingsException &e) {
		logErr(e.what());
		return false;
	}
	if (temporary == false) {
		file::rm(_getFileName(true), true);
		_saved = true;
	}
	return true;
}

void	Save::deleteTemp() {
	file::rm(get()._getFileName(true), true);
}

// -- Exceptions errors --------------------------------------------------------

Save::SaveException::SaveException()
: std::runtime_error("Save Exception") {}

Save::SaveException::SaveException(const char* what_arg)
: std::runtime_error(std::string(std::string("SaveError: ") + what_arg).c_str()) {}
