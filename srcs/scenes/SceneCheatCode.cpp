#include <unistd.h>
#include <iostream>
#include <fstream>
#include <string>
#include <regex>
#include "SceneCheatCode.hpp"
#include "SceneGame.hpp"
#include "FileUtils.hpp"

SceneCheatCode::SceneCheatCode(Gui * gui, float const &dtTime)
: ASceneMenu(gui, dtTime),
  isCmdLnEnabled(true),
  _historyActID(-1)
{
	_commandsList = {
		{"help", {
			"[command, ...]",
			"get general help or help on a command",
			&SceneCheatCode::_execHelp,
		}},
		{"clear", {
			"[history] [all]",
			"clear the lines / history / ...",
			&SceneCheatCode::_execClear,
		}},
		{"log", {
			"<type> <message>",
			"Log a message. Type: debug, info, success, warn, err, fatal",
			&SceneCheatCode::_execLog,
		}},
		{"tp", {
			"<x> <y>",
			"Teleport to a given position (if possible)",
			&SceneCheatCode::_execTp,
		}},
	};
}

SceneCheatCode::SceneCheatCode(SceneCheatCode const & src)
: ASceneMenu(src)
{
	*this = src;
}

SceneCheatCode::~SceneCheatCode() {
	/* save history */
	file::rm(CHEATCODE_HIST_FILE);
	std::ofstream	file(CHEATCODE_HIST_FILE);

	if (file.is_open()) {
		for (auto && line : _cmdHistory) {
			file << line << "\n";
		}
		file.close();
	}
	else {
		logWarn("unable to save cheatcode history");
	}
}

SceneCheatCode & SceneCheatCode::operator=(SceneCheatCode const & rhs) {
	if (this != &rhs) {
		logWarn("you are copying SceneCheatCode")
	}
	return *this;
}

/**
 * @brief init the menu
 *
 * @return true if the init succed
 * @return false if the init failed
 */
bool			SceneCheatCode::init() {
	/* load history */
	std::ifstream	file(CHEATCODE_HIST_FILE);
	std::string		line;

	if (file.is_open()) {
		for (std::string line; std::getline(file, line); ) {
			_cmdHistory.push_back(line);
		}
		file.close();
	}
	else {
		logDebug("unable to load cheatcode history");
	}

	/* create UI */
	glm::vec2 winSz = _gui->gameInfo.windowSize;
	glm::vec2 tmpPos;
	glm::vec2 tmpSize;

	try {
		tmpPos.x = 0;
		tmpSize.x = winSz.x;
		tmpSize.y = ABaseUI::strHeight(CHEATCODE_FONT, CHEATCODE_FONT_SCALE) * 1.4;
		tmpPos.y = tmpSize.y;
		_commandLine = &addTextInput(tmpPos, tmpSize, "/help to get help");
		_commandLine->setAlwaysFocus(true)
			.setTextFont(CHEATCODE_FONT)
			.setTextScale(CHEATCODE_FONT_SCALE)
			.setTextColor(CHEATCODE_TEXT_COlOR)
			.setText(CHEATCODE_DEF_TXT)
			.setColor(CHEATCODE_COLOR)
			.setZ(1);
	}
	catch (ABaseUI::UIException const & e) {
		logErr(e.what());
		return false;
	}
	return true;
}

/**
 * @brief this is the update function (called every frames)
 *
 * @return true if the update is a success
 * @return false if we need to quit the command line
 */
bool	SceneCheatCode::update() {
	ASceneMenu::update();

	if (isCmdLnEnabled) {
		_commandLine->setFocus(false);
		/* go to history */
		if (_cmdHistory.size() > 0) {
			if (Inputs::getKeyByScancodeDown(SDL_SCANCODE_UP)) {
				if (_historyActID < static_cast<int>(_cmdHistory.size() - 1)) {
					if (_historyActID == -1) {
						_historySavedLine = _commandLine->getText();  // save line
					}
					_historyActID += 1;
					_commandLine->setText(_cmdHistory[_cmdHistory.size() - 1 - _historyActID]);
				}
			}
			else if (Inputs::getKeyByScancodeDown(SDL_SCANCODE_DOWN)) {
				if (_historyActID > -1) {
					_historyActID -= 1;
					if (_historyActID < 0) {
						_commandLine->setText(_historySavedLine);
					}
					else {
						_commandLine->setText(_cmdHistory[_cmdHistory.size() - 1 - _historyActID]);
					}
				}
			}
		}

		/* exec command */
		_commandLine->setFocus(true);
		if (Inputs::getKeyByScancodeUp(SDL_SCANCODE_RETURN)) {
			_historyActID = -1;
			_historySavedLine = "";
			if (evalCommand(_commandLine->getText()))
				return false;  // close command line
		}
	}
	else {
		_commandLine->setFocus(false);
	}

	_commandLine->setEnabled(isCmdLnEnabled);

	/* exit command line */
	if (isCmdLnEnabled) {
		_commandLine->setFocus(false);
		if (Inputs::getKeyUp(InputType::CANCEL)) {
			_gui->disableExitForThisFrame(true);
			return false;
		}
		_commandLine->setFocus(true);
	}

	return true;
}

/**
 * @brief called when the scene is loaded
 */
void SceneCheatCode::load() {
	ASceneMenu::load();
	_historySavedLine = "";
	_historyActID = -1;
	_commandLine->setFocus(true);
}

/**
 * @brief called when the scene is unloaded
 */
void SceneCheatCode::unload() {
	ASceneMenu::unload();
	_commandLine->setFocus(false);
}

/**
 * @brief Parse & execute a command
 *
 * @param command The command
 * @return true If the command is a success
 * @return false If we need to keep the command line open (command fail for example)
 */
bool SceneCheatCode::evalCommand(std::string const & command) {
	int ret = CheatcodeAction::KEEP_OPEN | CheatcodeAction::TXT_DEF | CheatcodeAction::CHEAT_NO_TXT_ONLY;

	if (command.size() > 0) {
		if (command[0] == '/') {
			std::vector<std::string> splittedCmd = _splitCommand(command);
			if (splittedCmd.empty()) {  // command is empty
				ret = CheatcodeAction::KEEP_OPEN | CheatcodeAction::TXT_DEF;  // keep command line open
			}
			else {  // if there is a command
				if (_isValidCommand(splittedCmd[0])) {  // if the command is valid
					ret = (*this.*_commandsList[splittedCmd[0]].exec)(splittedCmd);
				}
				else {  // if the command is invalid
					this->logerr("Invalid command: " + splittedCmd[0] + " (try /help)", false, true);
					ret = CheatcodeAction::KEEP_OPEN | CheatcodeAction::TXT_KEEP;  // keep command line open
				}
			}
		}
		else {  // not a command
			_addLine(command);
			ret = CheatcodeAction::KEEP_OPEN | CheatcodeAction::TXT_RESET;  // keep command line open
		}

		/* add in history */
		if (command != "/" && command[0] != ' ') {  // don't save `/` only or line that start with space
			_cmdHistory.push_back(command);
			while (_cmdHistory.size() > s.j("cheatcode").u("historySize")) {
				_cmdHistory.pop_front();
			}
		}
	}

	/* txt actions */
	if (ret & CheatcodeAction::TXT_DEF) {
		_commandLine->setText(CHEATCODE_DEF_TXT);
	}
	else if (ret & CheatcodeAction::TXT_KEEP) {
	}
	else if (ret & CheatcodeAction::TXT_RESET) {
		_commandLine->inputReset();
	}

	/* text only option action */
	if (ret & CheatcodeAction::CHEAT_NO_TXT_ONLY) {
		SceneManager::openCheatCodeForTime(0);  // disable only text option
	}
	else if (ret & CheatcodeAction::CHEAT_TXT_ONLY && ret & CheatcodeAction::CLOSE) {
		SceneManager::openCheatCodeForTime(s.j("cheatcode").u("timeLineShow"));  // show lines for x seconds
	}

	/* close after update */
	if (ret & CheatcodeAction::KEEP_OPEN)
		return false;
	return true;
}

/**
 * @brief Clear all lines
 */
void SceneCheatCode::clearAllLn() {
	while (_textLines.size() > 0) {
		_removeLastLine();
	}
}

/**
 * @brief Set the command line text
 *
 * @param txt The text
 */
void SceneCheatCode::setText(std::string const & txt) {
	_commandLine->setText(txt);
}

/**
 * @brief Get the text in command line
 *
 * @return std::string The text in command line
 */
std::string SceneCheatCode::getText() const {
	return _commandLine->getText();
}

/**
 * @brief Split a command in separate words (you can use quote `"`)
 *
 * @param command The command to split
 * @return std::vector<std::string> a vector with all strings
 */
std::vector<std::string> SceneCheatCode::_splitCommand(std::string const & command) {
	std::vector<std::string> splitted;
	if (command.size() == 0)
		return splitted;

	uint32_t start = 0;
	uint32_t size = 0;

	if (command[0] == '/')
		start = 1;

	bool isQuoted;
	while (start + size < command.size()) {
		/* get a word */
		isQuoted = false;

		// remove spaces
		while (start + size < command.size() && _isSpace(command[start])) {
			start += 1;
		}
		if (command[start] == '"') {
			isQuoted = true;
			start++;
		}
		while (start + size < command.size()) {
			if (isQuoted) {
				if (command[start + size] == '"' && (size == 0 || command[start + size - 1] != '\\'))
					break;
			}
			else {
				if (command[start + size] == '"' && (size == 0 || command[start + size - 1] != '\\')) {
					this->logerr("invalid quote matching: to in sert quote in a word, use \\\"", false, true);
					return std::vector<std::string>();
				}
				if (_isSpace(command[start + size]))
					break;
			}
			size += 1;
		}
		if (size > 0) {
			if (isQuoted && command[start + size] != '"') {
				this->logerr("invalid quote matching", false, true);
				return std::vector<std::string>();
			}
			std::string word = command.substr(start, size);
			// "\\" to "\"
			word = std::regex_replace(word, std::regex("\\\\\\\\"), "\\");
			// \" to "
			word = std::regex_replace(word, std::regex("\\\\\""), "\"");
			// \t to tab
			word = std::regex_replace(word, std::regex("\\\\t"), CHEATCODE_TAB);
			// "\n" to '\n'
			word = std::regex_replace(word, std::regex("\\\\n"), "\n");
			splitted.push_back(word);
			start += size + (isQuoted ? 1 : 0);
		}
		size = 0;
	}

	if (splitted.empty()) {
		_commandLine->inputReset();
	}

	return splitted;
}

bool SceneCheatCode::_isSpace(char c) const {
	if (c == ' ' || c == '\t')
		return true;
	return false;
}

/**
 * @brief Check if a command name is valid
 *
 * @param name The command name
 * @return true If it's a valid name
 */
bool SceneCheatCode::_isValidCommand(std::string const & name) const {
	if (_commandsList.find(name) != _commandsList.end())
		return true;
	return false;
}

int64_t SceneCheatCode::_toInt(std::string const & arg, bool & error) const {
	error = false;
	if (std::regex_match(arg, REGEX_INT) == false) {
		error = true;
		return 0;
	}
	int64_t val = static_cast<int64_t>(std::atoi(arg.c_str()));
	if (std::atof(arg.c_str()) > static_cast<double>(std::numeric_limits<int64_t>::max())
	|| std::atof(arg.c_str()) < static_cast<double>(std::numeric_limits<int64_t>::min())) {
		error = true;
		return 0;
	}
	return val;
}
uint64_t SceneCheatCode::_toUint(std::string const & arg, bool & error) const {
	error = false;
	if (std::regex_match(arg, REGEX_UINT) == false) {
		error = true;
		return 0;
	}
	uint64_t val = static_cast<uint64_t>(std::atoi(arg.c_str()));
	if (std::atof(arg.c_str()) > static_cast<double>(std::numeric_limits<uint64_t>::max())
	|| std::atof(arg.c_str()) < static_cast<double>(std::numeric_limits<uint64_t>::min())) {
		error = true;
		return 0;
	}
	return val;
}
double SceneCheatCode::_toFloat(std::string const & arg, bool & error) const {
	error = false;
	if (std::regex_match(arg, REGEX_FLOAT) == false) {
		error = true;
		return 0;
	}
	double val = static_cast<double>(std::atof(arg.c_str()));
	if (!isfinite(val)) {
		error = true;
		return 0;
	}
	return val;
}

/**
 * @brief Add a line in cheatcode
 *
 * @param txt The line text
 * @param txtColor The line text color
 */
void SceneCheatCode::_addLine(std::string const & txt, glm::vec4 txtColor) {
	glm::ivec2 tmpPos;

	tmpPos = _commandLine->getPos();
	tmpPos.y += _commandLine->getSize().y * 1.3;
	TextLine newLine;

	std::stringstream ss(txt);
	std::string line;

	while (std::getline(ss, line, '\n')) {
		newLine.ui = &addText(tmpPos, _commandLine->getSize(), line);
		newLine.ui->setTextAlign(TextAlign::LEFT)
			.setTextFont(CHEATCODE_FONT)
			.setTextScale(CHEATCODE_FONT_SCALE)
			.setTextColor(txtColor)
			.setColor(CHEATCODE_COLOR)
			.setZ(1);

		for (auto && ln : _textLines) {
			ln.ui->addPosOffset({0, ln.ui->getSize().y});
		}

		_textLines.push_front(newLine);
	}

	while (_textLines.size() > s.j("cheatcode").u("maxLinesShow")) {
		_removeLastLine();
	}
}

/**
 * @brief Remove the last line in cheat code
 */
void SceneCheatCode::_removeLastLine() {
	TextLine const & last = _textLines.back();

	for (auto it = _buttons.begin(); it != _buttons.end(); it++) {
		if (*it == last.ui) {
			_buttons.erase(it);
			break;
		}
	}

	_textLines.pop_back();
}

/**
 * Logging
 */

/**
 * @brief Log a message on the screen
 *
 * @param msg The message
 * @param clear Clear lines before log
 * @param logOnly Log only (false if called internally)
 */
void SceneCheatCode::logdebug(std::string const & msg, bool clear, bool logOnly) {
	#if DEBUG
		if (clear)
			clearAllLn();
		_addLine(msg, CHEATCODE_TEXT_COlOR_DEBUG);
		logDebug(msg);
		if (!logOnly) {
			SceneManager::openCheatCodeForTime(s.j("cheatcode").u("timeLineShow"));  // show lines for x seconds
		}
	#else
		(void)msg;
		(void)clear;
		(void)logOnly;
	#endif
}
/**
 * @brief Log a message on the screen
 *
 * @param msg The message
 * @param clear Clear lines before log
 * @param logOnly Log only (false if called internally)
 */
void SceneCheatCode::loginfo(std::string const & msg, bool clear, bool logOnly) {
	if (clear)
		clearAllLn();
	_addLine(msg, CHEATCODE_TEXT_COlOR_INFO);
	logInfo(msg);
	if (!logOnly) {
		SceneManager::openCheatCodeForTime(s.j("cheatcode").u("timeLineShow"));  // show lines for x seconds
	}
}
/**
 * @brief Log a message on the screen
 *
 * @param msg The message
 * @param clear Clear lines before log
 * @param logOnly Log only (false if called internally)
 */
void SceneCheatCode::logsuccess(std::string const & msg, bool clear, bool logOnly) {
	if (clear)
		clearAllLn();
	_addLine(msg, CHEATCODE_TEXT_COlOR_SUCCESS);
	logSuccess(msg);
	if (!logOnly) {
		SceneManager::openCheatCodeForTime(s.j("cheatcode").u("timeLineShow"));  // show lines for x seconds
	}
}
/**
 * @brief Log a message on the screen
 *
 * @param msg The message
 * @param clear Clear lines before log
 * @param logOnly Log only (false if called internally)
 */
void SceneCheatCode::logwarn(std::string const & msg, bool clear, bool logOnly) {
	if (clear)
		clearAllLn();
	_addLine(msg, CHEATCODE_TEXT_COlOR_WARN);
	logWarn(msg);
	if (!logOnly) {
		SceneManager::openCheatCodeForTime(s.j("cheatcode").u("timeLineShow"));  // show lines for x seconds
	}
}
/**
 * @brief Log a message on the screen
 *
 * @param msg The message
 * @param clear Clear lines before log
 * @param logOnly Log only (false if called internally)
 */
void SceneCheatCode::logerr(std::string const & msg, bool clear, bool logOnly) {
	if (clear)
		clearAllLn();
	_addLine(msg, CHEATCODE_TEXT_COlOR_ERR);
	logErr(msg);
	if (!logOnly) {
		SceneManager::openCheatCodeForTime(s.j("cheatcode").u("timeLineShow"));  // show lines for x seconds
	}
}
/**
 * @brief Log a message on the screen
 *
 * @param msg The message
 * @param clear Clear lines before log
 * @param logOnly Log only (false if called internally)
 */
void SceneCheatCode::logfatal(std::string const & msg, bool clear, bool logOnly) {
	if (clear)
		clearAllLn();
	_addLine(msg, CHEATCODE_TEXT_COlOR_FATAL);
	logFatal(msg);
	if (!logOnly) {
		SceneManager::openCheatCodeForTime(s.j("cheatcode").u("timeLineShow"));  // show lines for x seconds
	}
}
