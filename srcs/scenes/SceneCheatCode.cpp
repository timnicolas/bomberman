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
	_draw3dMenu = false;
	_commandsList = {
		{"help", {
			"[command, ...]",
			"Get general help or help on a command.",
			&SceneCheatCode::_execHelp,
		}},
		{"clear", {
			"['history'] ['all']",
			"Clear the lines / history / ... (/clear list to have more informations).",
			&SceneCheatCode::_execClear,
		}},
		{"log", {
			"<type> <message>",
			"Log a message (/log list to get the list of logs types).\n"
				CHEATCODE_TAB"/log info \"info message\"",
			&SceneCheatCode::_execLog,
		}},
		{"tp", {
			"<x> <y>",
			"Teleport to a given position (if possible). You can set relative position with ~ (~-1, ~3, ...)\n"
				CHEATCODE_TAB"/tp 3 ~ -> tp 3 block right to you",
			&SceneCheatCode::_execTp,
		}},
		{"getbonus", {
			"<bonus, ...> ['list']",
			"Get a bonus effect ('/getbonus list' to get the list of bonus)",
			&SceneCheatCode::_execGetbonus,
		}},
		{"loop", {
			"<iter> <commands ...>",
			"Exec commands multiples times\n"
				CHEATCODE_TAB"/loop 3 \"/clear\" \"/getbonus life\"",
			&SceneCheatCode::_execLoop,
		}},
		{"exec", {
			"<commands ...>",
			"Execute multiples commands\n"
				CHEATCODE_TAB"/exec \"/clear\" \"/log info \\\"cleared\\\"\"",
			&SceneCheatCode::_execExec,
		}},
		{"summon", {
			"<typename> <x> <y>",
			"Summon a block, entity, ... ('/summon list' to show the full list of entity)\n"
				CHEATCODE_TAB"/summon enemyBasic ~3 5",
			&SceneCheatCode::_execSummon,
		}},
		{"unlock", {
			"<levelId ...>",
			"Unlock a level by id\n",
			&SceneCheatCode::_execUnlock,
		}},
		{"rmbonus", {
			"<bonus, ...> ['list']",
			"Remove a bonus effect ('/rmbonus list' to get the list of bonus)",
			&SceneCheatCode::_execRmbonus,
		}},
		{"restart", {
			"",
			"Restart the level",
			&SceneCheatCode::_execRestart,
		}},
		{"debug", {
			"<type> <element ...> ['list']",
			"Show or hide debug elements.\n"
				CHEATCODE_TAB"type: 'show' or 'hide'\n"
				CHEATCODE_TAB"/debug show collider",
			&SceneCheatCode::_execDebug,
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
 * @return true if the init succeed
 * @return false if the init failed
 */
bool SceneCheatCode::init() {
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
		tmpPos.x = 5;
		tmpSize.x = winSz.x - (tmpPos.x * 2);
		tmpSize.y = ABaseUI::strHeight(CHEATCODE_FONT, CHEATCODE_FONT_SCALE) * 1.6;

		tmpPos.y = tmpSize.y / 2;
		_infoCommandLine = &addText(tmpPos, tmpSize, "");
		_infoCommandLine->setEnabled(false)
			.setTextFont(CHEATCODE_FONT)
			.setTextScale(CHEATCODE_FONT_SCALE)
			.setTextColor(CHEATCODE_TEXT_COlOR_DEBUG)
			.setText(CHEATCODE_DEF_TXT)
			.setColor(CHEATCODE_COLOR)
			.setTextAlign(TextAlign::LEFT)
			.setZ(1);

		tmpPos.y += tmpSize.y;
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
bool SceneCheatCode::update() {
	ASceneMenu::update();

	if (isCmdLnEnabled) {
		_commandLine->setFocus(false);
		/* go to history */
		_history();  // to be called with _commandLine->setFocus(false);

		/* autocompletion */
		_autocompletion();  // to be called with _commandLine->setFocus(false);
		_commandLine->setFocus(true);

		/* exec command */
		if (Inputs::getKeyByScancodeUp(SDL_SCANCODE_RETURN)) {
			_historyActID = -1;
			_historySavedLine = "";
			if (evalCommand(_commandLine->getText()) & CheatcodeAction::CLOSE)
				return false;  // close command line
		}
	}
	else {
		_infoCommandLine->setEnabled(false);
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
 * @param ignoreHistory If true, don't add the command to history
 * @return true If the command is a success
 * @return false If we need to keep the command line open (command fail for example)
 */
int SceneCheatCode::evalCommand(std::string const & command, bool ignoreHistory) {
	int ret = CheatcodeAction::KEEP_OPEN | CheatcodeAction::TXT_DEF | CheatcodeAction::CHEAT_NO_TXT_ONLY
		| CheatcodeAction::RESULT_SUCCESS;

	if (command.size() > 0) {
		bool isCommand;
		_getCommandName(command, isCommand);  // get if it's a command
		if (isCommand) {
			std::vector<std::string> splittedCmd = _splitCommand(command);
			if (splittedCmd.empty()) {  // command is empty
				ret = CheatcodeAction::KEEP_OPEN | CheatcodeAction::TXT_KEEP | CheatcodeAction::RESULT_ERROR;
			}
			else {  // if there is a command
				if (_isValidCommand(splittedCmd[0])) {  // if the command is valid
					ret = (*this.*_commandsList[splittedCmd[0]].exec)(splittedCmd);
				}
				else {  // if the command is invalid
					this->logerr("Invalid command: " + splittedCmd[0] + " (try /help)", false, true);
					ret = CheatcodeAction::KEEP_OPEN | CheatcodeAction::TXT_KEEP | CheatcodeAction::RESULT_ERROR;
				}
			}
		}
		else {  // not a command
			_addLine(command);
			ret = CheatcodeAction::KEEP_OPEN | CheatcodeAction::TXT_RESET | CheatcodeAction::RESULT_SUCCESS;
		}

		/* add in history */
		if (!ignoreHistory && command != "/" && command[0] != ' ') {  // don't save `/` only or line that start with space
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

	/* command result */
	if (ret & CheatcodeAction::RESULT_ERROR) {
		// error
	}
	else if (ret & CheatcodeAction::RESULT_SUCCESS) {
		// success
	}

	return ret;
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
 * @brief Go to the history if needed -> need to call _commandLine->setFocus(false); before
 */
void SceneCheatCode::_history() {
	if (_cmdHistory.size() > 0) {
		switch (Inputs::getTextInputKeycode()) {
			case SDLK_UP:
				if (_historyActID < static_cast<int>(_cmdHistory.size() - 1)) {
					if (_historyActID == -1) {
						_historySavedLine = _commandLine->getText();  // save line
					}
					_historyActID += 1;
					_commandLine->setText(_cmdHistory[_cmdHistory.size() - 1 - _historyActID]);
				}
				break;
			case SDLK_DOWN:
				if (_historyActID > -1) {
					_historyActID -= 1;
					if (_historyActID < 0) {
						_commandLine->setText(_historySavedLine);
					}
					else {
						_commandLine->setText(_cmdHistory[_cmdHistory.size() - 1 - _historyActID]);
					}
				}
				break;
		}
	}
}

/**
 * @brief Compute the autocompletion if needed -> need to call _commandLine->setFocus(false); before
 */
void SceneCheatCode::_autocompletion() {
	bool						isCommand;
	std::string					cmdName = _getCommandName(_commandLine->getText(), isCommand);
	std::vector<std::string>	possibility;
	bool						disableInfoCmdLn = true;

	if (!isCommand) {
		_infoCommandLine->setEnabled(false);
		return;
	}

	/* if command line is /text (with invalid or incomplete command) */
	// check what we can complete
	if (!_isValidCommand(cmdName)) {  // only one arg (start of the command name)
		for (auto && cmd : _commandsList) {
			if (cmd.first.rfind(cmdName, 0) == 0) {
				possibility.push_back(cmd.first);
			}
		}
		if (possibility.size() >= 1) {
			std::string txt;
			for (auto && it : possibility) {
				if (txt.size() > 0)
					txt += CHEATCODE_TAB;
				txt += it;
			}
			_infoCommandLine->setText(txt).setTextColor(CHEATCODE_TEXT_COlOR_DEBUG).setEnabled(true);
			disableInfoCmdLn = false;
		}
	}

	/* if it's a command (valid or invalid) */
	if (disableInfoCmdLn) {
		if (_isValidCommand(cmdName)) {  // valid command
			std::string ln;
			ln = "/" + cmdName;
			if (_commandsList[cmdName].prototype.size() > 0)
				ln += " " + _commandsList[cmdName].prototype;
			_infoCommandLine->setText(ln).setTextColor(CHEATCODE_TEXT_COlOR_DEBUG).setEnabled(true);
			disableInfoCmdLn = false;
		}
		else {  // invalid command
			std::string ln = "Invalid command /" + cmdName + " (try /help)";
			_infoCommandLine->setText(ln).setTextColor(CHEATCODE_TEXT_COlOR_ERR).setEnabled(true);
			disableInfoCmdLn = false;
		}
	}

	/* process tab */
	if (Inputs::getKeyByScancodeUp(SDL_SCANCODE_TAB)) {
		/* if text is empty */
		if (_commandLine->getText().size() == 0) {  // ""
			_commandLine->setText(CHEATCODE_DEF_TXT);
		}
		/* if we can complete command name */
		else if (possibility.size() == 1) {
			cmdName = possibility[0];
			_commandLine->setText("/" + cmdName + " ");
			std::string ln;
			ln = "/" + cmdName;
			if (_commandsList[cmdName].prototype.size() > 0)
				ln += " " + _commandsList[cmdName].prototype;
			_infoCommandLine->setText(ln).setTextColor(CHEATCODE_TEXT_COlOR_DEBUG).setEnabled(true);
			disableInfoCmdLn = false;
		}
	}
	if (disableInfoCmdLn) {
		_infoCommandLine->setEnabled(false);
	}
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


	while (start < command.size() && _isSpace(command[start])) {
		start += 1;
	}
	if (start < command.size() && command[start] == '/') {
		start += 1;
	}

	if (start >= command.size())
		return splitted;

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
					this->logerr("invalid quote matching: to insert quote in a word, use \\\"", false, true);
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

int64_t SceneCheatCode::_toInt(std::string const & arg, bool & error, bool * isRelative) {
	error = false;
	if (isRelative != nullptr)
		*isRelative = false;

	std::string tmpNb = arg;
	if (arg.size() > 0 && arg[0] == '~') {
		if (isRelative != nullptr) {
			*isRelative = true;
			tmpNb = arg.substr(1, arg.size() - 1);
			if (arg.size() == 1) {  // only "~"
				return 0;  // result if pos relative + 0
			}
		}
		else {
			this->logerr("Cannot use relative number for this function", false, true);
			error = true;
			return 0;
		}
	}
	if (std::regex_match(tmpNb, REGEX_INT) == false) {
		error = true;
		return 0;
	}
	int64_t val = static_cast<int64_t>(std::atoi(tmpNb.c_str()));
	if (std::atof(tmpNb.c_str()) > static_cast<double>(std::numeric_limits<int64_t>::max())
	|| std::atof(tmpNb.c_str()) < static_cast<double>(std::numeric_limits<int64_t>::min())) {
		error = true;
		return 0;
	}
	return val;
}
uint64_t SceneCheatCode::_toUint(std::string const & arg, bool & error, bool * isRelative) {
	error = false;
	if (isRelative != nullptr)
		*isRelative = false;

	std::string tmpNb = arg;
	if (arg.size() > 0 && arg[0] == '~') {
		if (isRelative != nullptr) {
			*isRelative = true;
			tmpNb = arg.substr(1, arg.size() - 1);
			if (arg.size() == 1) {  // only "~"
				return 0;  // result if pos relative + 0
			}
		}
		else {
			this->logerr("Cannot use relative number for this function", false, true);
			error = true;
			return 0;
		}
	}
	if (std::regex_match(tmpNb, REGEX_UINT) == false) {
		error = true;
		return 0;
	}
	uint64_t val = static_cast<uint64_t>(std::atoi(tmpNb.c_str()));
	if (std::atof(tmpNb.c_str()) > static_cast<double>(std::numeric_limits<uint64_t>::max())
	|| std::atof(tmpNb.c_str()) < static_cast<double>(std::numeric_limits<uint64_t>::min())) {
		error = true;
		return 0;
	}
	return val;
}
double SceneCheatCode::_toFloat(std::string const & arg, bool & error, bool * isRelative) {
	error = false;
	if (isRelative != nullptr)
		*isRelative = false;

	std::string tmpNb = arg;
	if (arg.size() > 0 && arg[0] == '~') {
		if (isRelative != nullptr) {
			*isRelative = true;
			tmpNb = arg.substr(1, arg.size() - 1);
			if (arg.size() == 1) {  // only "~"
				return 0;  // result if pos relative + 0
			}
		}
		else {
			this->logerr("Cannot use relative number for this function", false, true);
			error = true;
			return 0;
		}
	}
	if (std::regex_match(tmpNb, REGEX_FLOAT) == false) {
		error = true;
		return 0;
	}
	double val = static_cast<double>(std::atof(tmpNb.c_str()));
	if (!isfinite(val)) {
		error = true;
		return 0;
	}
	return val;
}

std::string SceneCheatCode::_getCommandName(std::string const & command, bool & isCommand) {
	isCommand = false;
	if (command.size() == 0)
		return "";

	uint32_t start = 0;
	uint32_t size = 0;

	while (start < command.size() && _isSpace(command[start])) {
		start += 1;
	}
	if (start < command.size() && command[start] == '/') {
		isCommand = true;
		start += 1;
	}
	else {
		return "";
	}
	if (start >= command.size())
		return "";

	// remove spaces
	while (start + size < command.size() && _isSpace(command[start])) {
		start += 1;
	}
	while (start + size < command.size()) {
		if (_isSpace(command[start + size]))
			break;
		size += 1;
	}
	if (size > 0) {
		std::string word = command.substr(start, size);
		return word;
	}
	return "";
}

/**
 * @brief Add a line in cheatcode
 *
 * @param txt The line text
 * @param txtColor The line text color
 *
 * @return The number of lines created
 */
int SceneCheatCode::_addLine(std::string const & txt, glm::vec4 txtColor) {
	int nbLines = 0;
	glm::ivec2 tmpPos;

	tmpPos = _commandLine->getPos();
	tmpPos.y += _commandLine->getSize().y * 1.3;
	TextLine newLine;

	std::stringstream ss(txt);
	std::string line;

	while (std::getline(ss, line, '\n')) {
		std::vector<std::string> allLines;
		if (ABaseUI::strWidth(CHEATCODE_FONT, line, CHEATCODE_FONT_SCALE)
		< _commandLine->getSize().x - UI_DEF_TEXT_PADDING)
		{
			allLines.push_back(line);
		}
		else {
			// split in multiples lines
			std::string ln;
			uint32_t start = 0;
			while (start < line.size()) {
				uint32_t size = 1;
				while (start + size < line.size()
				&& ABaseUI::strWidth(CHEATCODE_FONT, line.substr(start, size), CHEATCODE_FONT_SCALE)
				< _commandLine->getSize().x - UI_DEF_TEXT_PADDING)
				{
					size += 1;
				}
				if (start + size < line.size())
					size -= 1;
				allLines.push_back(line.substr(start, size));
				start += size;
			}
		}
		for (auto && lineText : allLines) {
			nbLines += 1;
			newLine.ui = &addText(tmpPos, _commandLine->getSize(), lineText);
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
	}

	while (_textLines.size() > s.j("cheatcode").u("maxLinesShow")) {
		_removeLastLine();
	}
	return nbLines;
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
 * @brief Check if a level was unlocked by cheat code
 *
 * @param levelId The level id to check
 * @return true If the level was unlocked
 */
bool SceneCheatCode::isLevelUnlocked(uint32_t levelId) {
	SceneCheatCode & scCheatCode = *reinterpret_cast<SceneCheatCode *>(SceneManager::getScene(SceneNames::CHEAT_CODE));
	return scCheatCode._isLevelUnlocked(levelId);
}
bool SceneCheatCode::_isLevelUnlocked(uint32_t levelId) const {
	return std::find(_levelsUnlocked.begin(), _levelsUnlocked.end(), levelId) != _levelsUnlocked.end();
}

/**
 * @brief Public method to unlock a level
 *
 * @param levelId level to unlock
 * @return int CheatcodeAction
 */
int	SceneCheatCode::unlockLevel(uint32_t levelId) {
	std::vector<std::string> args = {"unlock", std::to_string(levelId)};
	return _execUnlock(args);
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
