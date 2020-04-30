#include <array>
#include <regex>

#include "Save.hpp"
#include "FileUtils.hpp"

#include "ACharacter.hpp"
#include "Player.hpp"

// -- Constructors -------------------------------------------------------------
/**
 * @brief Private constructor. Construct a new Save:: Save object
 *
 */
Save::Save() {
	_instantiate = false;
	_saved = false;
}

/**
 * @brief Destroy the Save:: Save object
 *
 */
Save::~Save() {
	delete _saveJs;
}

/**
 * @brief Private copy constructor. Construct a new Save:: Save object
 *
 * @param src
 */
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

// -- initiate methods ---------------------------------------------------------

/**
 * @brief Load filename to actual save.
 *
 * @param filename
 * @return Save& this
 */
Save	&Save::loadGame(std::string filename) {
	return get()._loadGame(filename);
}
Save	&Save::_loadGame(std::string filename) {
	if (isInstantiate()) {
		delete _saveJs;
		deleteTemp();
	}
	_instantiate = false;
	_init();
	try {
		_saveJs = initJson(filename, false);
		_filename = filename;
		_instantiate = true;
		_time = filenameToTimestamp(filename, true);
	} catch (SaveException &e) {
		logErr(e.what());
		_instantiate = false;
	}
	_saved = true;

	return *this;
}

/**
 * @brief New Game create a new file with the current _time.
 *
 * @return Save& this
 */
Save	&Save::newGame() {
	return get()._newGame();
}
Save	&Save::_newGame() {
	if (isInstantiate()) {
		delete _saveJs;
		deleteTemp();
	}
	_instantiate = false;
	_init();
	_filename = _getFilename(false);
	try {
		_saveJs = initJson(_filename, true);
		_saveJs->saveToFile(_getFilename(true));
		_instantiate = true;
		_saved = false;
	} catch (SaveException &e) {
		logErr(e.what());
	} catch (SettingsJson::SettingsException &e) {
		logErr(e.what());
	}

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

/**
 * @brief Get filename
 *
 * @param temporary
 * @return std::string
 */
std::string	Save::getFilename(bool temporary) {
	return get()._getFilename(temporary);
}
std::string	Save::_getFilename(bool temporary) const {
	return SAVE_DIR + timestampToFileName(_time) + (temporary ? "_temp" : "") + ".save";
}

bool		Save::isInstantiate() {
	return get()._instantiate;
}

bool		Save::isSaved() {
	return get()._saved;
}

// -- Static Methods -----------------------------------------------------------

/**
 * @brief Get Regex to certificate a correct name file.
 *
 * @param temporary true if we want to include temporary filename.
 * @return std::string regex
 */
std::string		Save::getFileNameRegex(bool temporary) {
	std::string regex = "^(" + Save::_addRegexSlashes(SAVE_DIR) + "|)";
	regex += "((\\d{4})-(\\d{2})-(\\d{2})_(\\d{2})-(\\d{2})-(\\d{2}))";
	if (temporary)
		regex += "(_temp)?";
	regex += "\\.save$";

	return regex;
}

/**
 * @brief Define Json format to saved file.
 *
 * @param newFile true if we don't want to check the given file.
 * @return true
 * @return false
 */
SettingsJson	*Save::initJson(std::string filename, bool newFile) {
	SettingsJson	*jsFile = new SettingsJson();

	try {
		// file not found and not new file
		if (!newFile && file::isFile(filename) == false)
			throw SaveException((filename + " is not a file").c_str());

		// check filename viability
		std::time_t time = filenameToTimestamp(filename, false);
		jsFile->name(std::to_string(time)).description("Save file");
		jsFile->add<std::string>("Filename", timestampToFileName(time));

		// Save json definition
		jsFile->add<std::string>("name");
		jsFile->add<int64_t>("date_creation", time);
		std::time_t		t = std::time(nullptr);
		jsFile->add<int64_t>("date_lastmodified", t);

		SettingsJson	*levelPattern = new SettingsJson();
		levelPattern->add<int64_t>("id", 0).setMin(0);
		levelPattern->add<int64_t>("score", 0).setMin(0).setMax(99999999);
		jsFile->addList<SettingsJson>("levels", levelPattern);

		jsFile->add<SettingsJson>("state");
			jsFile->j("state").add<uint64_t>("life", 2).setMin(1).setMax(999);
			jsFile->j("state").add<uint64_t>("bombs", 1).setMin(1).setMax(999);
			jsFile->j("state").add<uint64_t>("flame", 2).setMin(1).setMax(999);
			jsFile->j("state").add<double>("speed", 3.0).setMin(3.0).setMax(MAX_SPEED);
			jsFile->j("state").add<uint64_t>("wallpass", 0).setMin(0).setMax(1);
			jsFile->j("state").add<uint64_t>("detonator", 0).setMin(0).setMax(1);
			jsFile->j("state").add<uint64_t>("bombpass", 0).setMin(0).setMax(1);
			jsFile->j("state").add<uint64_t>("flampass", 0).setMin(0).setMax(1);

		if (!newFile) {
			if (jsFile->loadFile(filename) == false) {
				delete jsFile;
				throw SaveException(("Fail loading " + filename).c_str());
			}
		}
	} catch (SettingsJson::SettingsException const & e) {
		delete jsFile;
		throw SaveException(e.what());
	}

	return jsFile;
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
	game.player->resetCrossable();
	game.player->lives = _saveJs->j("state").u("life");
	game.player->totalBombs = _saveJs->j("state").u("bombs");
	game.player->bombProgation = _saveJs->j("state").u("flame");
	game.player->speed = _saveJs->j("state").d("speed");
	if (_saveJs->j("state").u("detonator") == 1)
		game.player->takeBonus(Bonus::bonus["detonator"]);
	if (_saveJs->j("state").u("wallpass") == 1)
		game.player->takeBonus(Bonus::bonus["wallpass"]);
	if (_saveJs->j("state").u("bombpass") == 1)
		game.player->takeBonus(Bonus::bonus["bombpass"]);
	if (_saveJs->j("state").u("flampass") == 1)
		game.player->takeBonus(Bonus::bonus["flampass"]);

	return true;
}

/**
 * @brief Get next level to play
 *
 * @return int ID of the level.
 */
int		Save::getLastLevel() {
	return get()._getLastLevel();
}
int		Save::_getLastLevel() {
	int	maxLevel = 0;
	for (SettingsJson *level : _saveJs->lj("levels").list) {
		maxLevel = level->i("id") >= maxLevel ? level->i("id") + 1 : maxLevel;
	}
	return maxLevel;
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
 * @brief Set a level as done
 *
 * @param levelId The level ID
 * @param score The score
 * @return false If failed
 */
bool	Save::setLevelDone(int32_t levelId, int32_t score) {
	return get()._setLevelDone(levelId, score);
}
bool	Save::_setLevelDone(int32_t levelId, int32_t score) {
	bool exist = false;
	for (SettingsJson *level : _saveJs->lj("levels").list) {
		if (level->i("id") == levelId) {
			exist = true;
			break;
		}
	}
	if (!exist) {
		SettingsJson	*levelPattern = new SettingsJson();
		levelPattern->add<int64_t>("id", 0).setMin(0);
		levelPattern->add<int64_t>("score", 0).setMin(0).setMax(99999999);
		levelPattern->i("id") = levelId;
		levelPattern->i("score") = score;

		_saveJs->lj("levels").add(levelPattern);
	}

	return true;
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
		_saveJs->saveToFile(_getFilename(temporary));
	} catch (SettingsJson::SettingsException &e) {
		logErr(e.what());
		return false;
	}
	if (temporary == false) {
		file::rm(_getFilename(true), true);
		_saved = true;
	}

	return true;
}

/**
 * @brief Delete temporary file
 *
 */
void	Save::deleteTemp() {
	file::rm(get()._getFilename(true), true);
}

/**
 * @brief Convert the filename to timestamp
 *
 * @param filename
 * @param temporary true if we want to include temporary filename.
 * @return time_t time of filename.
 */
std::time_t	Save::filenameToTimestamp(std::string filename, bool temporary) {
	std::regex	regex(getFileNameRegex(temporary));
	std::smatch match;
	std::regex_match(filename, match, regex);
	if (!match.size())
		throw SaveException(("incorrect filename: " + filename).c_str());

	struct tm *timeInfo;
	std::time_t rawtime;
	time ( &rawtime );
	timeInfo = localtime ( &rawtime );

	timeInfo->tm_year = std::stoi(match[3]) - 1900;
	timeInfo->tm_mon = std::stoi(match[4]) - 1;
	timeInfo->tm_mday = std::stoi(match[5]);
	timeInfo->tm_hour = std::stoi(match[6]);
	timeInfo->tm_min = std::stoi(match[7]);
	timeInfo->tm_sec = std::stoi(match[8]);

	return mktime(timeInfo);
}

/**
 * @brief Return formated filename from a timestamp
 *
 * @param timestamp created time of file
 * @return std::string filename
 */
std::string	Save::timestampToFileName(time_t timestamp) {
	std::tm localTime = *std::localtime(&timestamp);
	std::stringstream ss;
	ss << std::put_time(&localTime, "%Y-%m-%d_%H-%M-%S");
	return ss.str();
}

// -- Private methods ----------------------------------------------------------

void	Save::_init() {
	_time = std::time(nullptr);
	_fileNameRegex = getFileNameRegex(true);
}

/**
 * @brief Add slashes to special regex char.
 *
 * @param str original string.
 * @return std::string corrected string
 */
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

// -- Exceptions errors --------------------------------------------------------

Save::SaveException::SaveException()
: std::runtime_error("Save Exception") {}

Save::SaveException::SaveException(const char* what_arg)
: std::runtime_error(std::string(std::string("SaveError: ") + what_arg).c_str()) {}
