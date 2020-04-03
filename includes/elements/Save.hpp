#ifndef SAVE_HPP_
#define SAVE_HPP_

#include <iostream>
#include <stdexcept>

#include "SettingsJson.hpp"
#include "SceneGame.hpp"

class Save {
private:
	// Members
	std::time_t		_time;
	std::string		_filename;
	SettingsJson	*_save;
	std::string		_fileNameRegex;
	// Methods
	void			_init();

public:
	// Constructors
	Save();
	explicit Save(std::string filename);
	~Save();
	Save(Save const &src);

	// Operators
	Save &				operator=(Save const &rhs);
	friend std::ostream& operator<<(std::ostream& os, const Save& my_class);

	// Getters & Setters
	std::string			getFileName(bool temporary);

	// Methods
	static std::string	addRegexSlashes(std::string str);
	bool				initJson();
	bool				updateSavedFile(SceneGame &game);
	bool				loadStatesSaved(SceneGame &game);
	bool				isLevelDone(int32_t levelId);
	bool				save(bool temporary);

	// Exceptions
	class SaveException : public std::runtime_error {
	public:
		SaveException();
		explicit SaveException(const char* what_arg);
	};
};

#endif  // SAVE_HPP_
