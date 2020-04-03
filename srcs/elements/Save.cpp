#include <array>
#include <regex>

#include "Save.hpp"
#include "FileUtils.hpp"

#include "ACharacter.hpp"
#include "Player.hpp"

// -- Constructors -------------------------------------------------------------

Save::Save() {
	_init();
	_save = new SettingsJson();
	initJson();
}

Save::Save(std::string filename) {
	_init();
	_save = new SettingsJson();
	std::regex	regex(_fileNameRegex);
	std::smatch	match;
	std::regex_match(filename, match, regex);
	if (match.size() >= 2) {
		_time = static_cast<time_t>(std::stoi(match[1]));
		if (file::isFile(filename) == false) {
			logInfo(filename << " not exist and will be created.");
		}
	} else {
		logErr("FileNameError: " << filename);
		logInfo("New file name: " << getFileName(false));
	}
	initJson();
}

Save::~Save() {
	delete _save;
}

Save::Save(Save const &src) {
	_save = new SettingsJson();
	*this = src;
}

// -- Operators ----------------------------------------------------------------

Save &Save::operator=(Save const &rhs) {
	if ( this != &rhs ) {
		_time = rhs._time;
		*_save = *(rhs._save);
		_fileNameRegex = rhs._fileNameRegex;
	}
	return *this;
}

std::ostream &	operator<<(std::ostream & os, const Save& my_class) {
	os << "Save: " << my_class._save;
	return (os);
}

// -- Getters & Setters --------------------------------------------------------

std::string	Save::getFileName(bool temporary) {
	return s.s("savePath") + std::to_string(_time) + (temporary ? "_temp" : "") + ".save";
}

// -- Private methods ----------------------------------------------------------

void	Save::_init() {
	_time = std::time(nullptr);
	_fileNameRegex = "^" + Save::addRegexSlashes(s.s("savePath")) + "(\\d{10,})(_temp)?\\.save$";
}

// -- Methods ------------------------------------------------------------------

std::string	Save::addRegexSlashes(std::string str) {
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
bool	Save::initJson() {
	try {
		std::string		saveName = std::to_string(_time);
		_filename = getFileName(false);
		_save->name(saveName).description("Save file");
		_save->add<std::string>("Filename", _filename);

		// Save json definition
		_save->add<std::string>("name");
		_save->add<int64_t>("date_creation", _time);
		std::time_t		t = std::time(nullptr);
		_save->add<int64_t>("date_lastmodified", t);

		SettingsJson	*levelPattern = new SettingsJson();
		levelPattern->add<int64_t>("id", 0).setMin(0);  // Todo(ebaudet): add setMax to nb levels.
		levelPattern->add<int64_t>("score", 0).setMin(0).setMax(99999999);
		_save->addList<SettingsJson>("levels", levelPattern);

		_save->add<SettingsJson>("state");
			_save->j("state").add<uint64_t>("life", 2).setMin(1).setMax(999);
			_save->j("state").add<uint64_t>("bombs", 1).setMin(1).setMax(999);
			_save->j("state").add<uint64_t>("flame", 1).setMin(1).setMax(999);
			_save->j("state").add<double>("speed", 3.0).setMin(3.0).setMax(MAX_SPEED);
			_save->j("state").add<uint64_t>("wallpass", 0).setMin(0).setMax(1);
			_save->j("state").add<uint64_t>("detonator", 0).setMin(0).setMax(1);
			_save->j("state").add<uint64_t>("bombpass", 0).setMin(0).setMax(1);
			_save->j("state").add<uint64_t>("flampass", 0).setMin(0).setMax(1);

		if (_save->loadFile(_filename) == false) {
			logErr("Fail loading " << _filename);
		}
	} catch (SettingsJson::SettingsException const & e) {
		logErr(e.what());
	}

	_save->saveToFile(getFileName(true));

	return true;
}

/**
 * @brief Update Saved File Informations
 *
 * @param game
 * @return true
 * @return false
 */
bool	Save::updateSavedFile(SceneGame &game) {
	std::cout << "LevelID: " << game.level << std::endl;
	std::cout << "LevelID: " << game.score << std::endl;

	_save->j("state").u("life") = game.player->lives;
	_save->j("state").u("bombs") = game.player->totalBombs;
	_save->j("state").u("flame") = game.player->bombProgation;
	_save->j("state").u("speed") = game.player->speed;
	_save->j("state").u("wallpass") = game.player->passWall == true ? 1 : 0;
	_save->j("state").u("detonator") = game.player->detonator == true ? 1 : 0;
	_save->j("state").u("bombpass") = game.player->passBomb == true ? 1 : 0;
	_save->j("state").u("flampass") = game.player->passFire == true ? 1 : 0;

	bool	levelAlreadyDone = false;
	for (SettingsJson *level : _save->lj("levels").list) {
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

		_save->lj("levels").add(levelPattern);
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
	game.player->lives = _save->j("state").u("life");
	game.player->totalBombs = _save->j("state").u("bombs");
	game.player->bombProgation = _save->j("state").u("flame");
	game.player->speed = _save->j("state").u("speed");
	game.player->passWall = _save->j("state").u("wallpass") == 0 ? false : true;
	game.player->detonator = _save->j("state").u("detonator") == 0 ? false : true;
	game.player->passBomb = _save->j("state").u("bombpass") == 0 ? false : true;
	game.player->passFire = _save->j("state").u("flampass") == 0 ? false : true;

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
	for (SettingsJson *level : _save->lj("levels").list) {
		if (level->i("id") == levelId) {
			return true;
		}
	}
	return false;
}

/**
 * @brief Save method to file.
 *
 * @param temporary
 * @return true
 * @return false
 */
bool	Save::save(bool temporary) {
	try {
		_save->saveToFile(getFileName(temporary));
	} catch (SettingsJson::SettingsException &e) {
		logErr(e.what());
		return false;
	}
	if (temporary == false) {
		file::rm(getFileName(true), true);
	}
	return true;
}

// -- Exceptions errors --------------------------------------------------------

Save::SaveException::SaveException()
: std::runtime_error("Save Exception") {}

Save::SaveException::SaveException(const char* what_arg)
: std::runtime_error(std::string(std::string("SaveError: ") + what_arg).c_str()) {}
