#ifndef SAVE_HPP_
#define SAVE_HPP_

#include <iostream>
#include <stdexcept>
#include <regex>

#include "SettingsJson.hpp"
#include "SceneGame.hpp"

/**
 * @brief class to manage saved games.
 *
 */
class Save {
public:
	// Constructors
	virtual			~Save();
	static Save		&get();

	static Save		&loadGame(std::string filename);
	static Save		&newGame();

	// Operators
	friend std::ostream& operator<<(std::ostream& os, const Save& my_class);

	// Getters & Setters
	static std::string	getFilename(bool temporary);
	static std::string	getFileNameRegex(bool temporary);

	static bool			isInstantiate();
	static bool			isSaved();
	// Methods
	static SettingsJson	*initJson(std::string filename, bool newFile = false);
	static bool			updateSavedFile(SceneGame &game, bool succeedLevel);
	static bool			loadStatesSaved(SceneGame &game);
	static bool			isLevelDone(int32_t levelId);
	static int			getLevelScore(int32_t levelId);
	static bool			setLevelDone(int32_t levelId, int32_t score);
	static void			setDifficulty(uint difficulty);
	static uint			getDifficulty();
	static bool			save(bool temporary = false);
	static void			deleteTemp();
	static std::time_t	filenameToTimestamp(std::string filename, bool temporary);
	static std::string	timestampToFileName(time_t timestamp);
	static int			getLastUnlockedLevel();
	// Exceptions
	/**
	 * @brief Save exception
	 */
	class SaveException : public std::runtime_error {
	public:
		SaveException();
		explicit SaveException(const char* what_arg);
	};

private:
	// Members
	bool			_saved;  /**< Is saved ? */
	bool			_instantiate;  /**< Is instanciate */
	std::time_t		_time;  /**< Time */
	std::string		_filename;  /**< Filename of save */
	SettingsJson	*_saveJs;  /**< Json save */
	std::string		_fileNameRegex;  /**< Filename regex */

	// Constructors
	Save();
	Save(Save const &src);

	// Operators
	Save &			operator=(Save const &rhs);

	// Methods
	Save				&_loadGame(std::string filename);
	Save				&_newGame();
	std::string			_getFilename(bool temporary) const;
	void				_init();
	bool				_isLevelDone(int32_t levelId);
	bool				_setLevelDone(int32_t levelId, int32_t score);
	int					_getLastUnlockedLevel();
	bool				_save(bool temporary);
	bool				_updateSavedFile(SceneGame &game, bool succeedLevel);
	bool				_loadStatesSaved(SceneGame &game);
	int					_getLevelScore(int32_t levelId);
	static std::string	_addRegexSlashes(std::string str);
};

#endif  // SAVE_HPP_
