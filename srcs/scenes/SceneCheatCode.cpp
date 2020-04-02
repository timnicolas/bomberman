#include "SceneCheatCode.hpp"
#include "SceneGame.hpp"

SceneCheatCode::SceneCheatCode(Gui * gui, float const &dtTime)
: ASceneMenu(gui, dtTime)
{}

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

	if (Inputs::getKeyByScancodeDown(SDL_SCANCODE_RETURN)) {
		std::string command = _commandLine->getText();
		if (command.size() > 0) {
			if (command[0] == '/')
				_addLine("invalid command: " + command, glm::vec4(1, 0, 0, 1));
			else
				_addLine(command);
			_commandLine->setText(CHEATCODE_DEF_TXT);
		}
		else {
			return false;
		}
	}

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
