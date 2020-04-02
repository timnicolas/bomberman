#include "SceneCheatCode.hpp"
#include "SceneGame.hpp"

SceneCheatCode::SceneCheatCode(Gui * gui, float const &dtTime)
: ASceneMenu(gui, dtTime),
  isCmdLnEnabled(true)
{
	_commandsList = {
		{"help", {
			"[command]",
			"get general help or help on a command",
			&SceneCheatCode::_execHelp,
		}},
		{"clear", {
			"",
			"clear the history",
			&SceneCheatCode::_execClear,
		}},
	};
}

SceneCheatCode::SceneCheatCode(SceneCheatCode const & src)
: ASceneMenu(src)
{
	*this = src;
}

SceneCheatCode::~SceneCheatCode() {}

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
		_commandLine->setFocus(true);
		if (Inputs::getKeyByScancodeDown(SDL_SCANCODE_RETURN)) {
			if (evalCommand(_commandLine->getText()))
				return false;  // close command line
		}
	}
	else {
		_commandLine->setFocus(false);
	}

	_commandLine->setEnabled(isCmdLnEnabled);

	return true;
}

/**
 * @brief called when the scene is loaded
 */
void SceneCheatCode::load() {
	ASceneMenu::load();
	_commandLine->setFocus(true);
}

/**
 * @brief called when the scene is unloaded
 */
void SceneCheatCode::unload() {
	ASceneMenu::unload();
	_commandLine->setText(CHEATCODE_DEF_TXT);
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
	bool ret = true;

	if (command.size() > 0) {
		if (command[0] == '/') {
			std::vector<std::string> splittedCmd = _splitCommand(command);
			if (splittedCmd.empty()) {  // command is empty
				ret = false;  // keep command line open
				_commandLine->inputReset();
			}
			else {  // if there is a command
				if (_isValidCommand(splittedCmd[0])) {  // if the command is valid
					ret = (*this.*_commandsList[splittedCmd[0]].exec)(splittedCmd);
					if (ret) {  // if command line close after command
						_commandLine->inputReset();
					}
					else {  // if command line keep open
						_commandLine->setText(CHEATCODE_DEF_TXT);
					}
				}
				else {  // if the command is invalid
					_addLine("invalid command: " + splittedCmd[0], glm::vec4(1, 0, 0, 1));
					ret = false;  // keep command line open
					_commandLine->setText(CHEATCODE_DEF_TXT);
				}
			}
		}
		else {  // not a command
			_addLine(command);
			ret = false;  // keep command line open
			_commandLine->inputReset();
		}
	}
	return ret;
}

std::vector<std::string> SceneCheatCode::_splitCommand(std::string const & command) const {
	std::vector<std::string> splitted;
	if (command.size() == 0)
		return splitted;

	uint32_t start = 0;
	uint32_t size = 0;

	if (command[0] == '/')
		start = 1;

	while (start + size < command.size()) {
		while (start + size < command.size() && _isSpace(command[start])) {
			start += 1;
		}
		while (start + size < command.size() && !_isSpace(command[start + size])) {
			size += 1;
		}
		if (size > 0) {
			splitted.push_back(command.substr(start, size));
			start += size;
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

	newLine.ui = &addText(tmpPos, _commandLine->getSize(), txt);
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
