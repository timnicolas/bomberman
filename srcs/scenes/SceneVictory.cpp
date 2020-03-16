#include "SceneVictory.hpp"
#include "SceneGame.hpp"

SceneVictory::SceneVictory(Gui * gui, float const &dtTime)
: ASceneMenu(gui, dtTime),
  _lastSceneName(SceneNames::MAIN_MENU)
{}

SceneVictory::SceneVictory(SceneVictory const & src)
: ASceneMenu(src)
{
	*this = src;
}

SceneVictory::~SceneVictory() {}

SceneVictory & SceneVictory::operator=(SceneVictory const & rhs) {
	if (this != &rhs) {
		logWarn("you are copying SceneVictory")
	}
	return *this;
}

/**
 * @brief init the menu
 *
 * @return true if the init succed
 * @return false if the init failed
 */
bool			SceneVictory::init() {
	glm::vec2 winSz = _gui->gameInfo.windowSize;
	glm::vec2 tmpPos;
	glm::vec2 tmpSize;
	float menuWidth = winSz.x / 2;
	float menuHeight = menuWidth / 8;

	try {
		tmpPos.x = (winSz.x / 2) - (menuWidth / 2);
		tmpPos.y = winSz.y - menuHeight * 2;
		tmpSize.x = menuWidth;
		tmpSize.y = menuHeight;
		addText(tmpPos, tmpSize, "Victory !").setTextFont("title");

		tmpPos.y -= menuHeight * 1.2;
		addButton(tmpPos, tmpSize, "NEXT LEVEL")
			.setKeyLeftClickInput(InputType::CONFIRM)
			.addButtonLeftListener(&_states.nextLevel);

		tmpPos.y -= menuHeight * 1.2;
		addButton(tmpPos, tmpSize, "RESTART")
			.addButtonLeftListener(&_states.restart);

		tmpPos.y -= menuHeight * 1.2;
		addButton(tmpPos, tmpSize, "MAIN MENU")
			.setKeyLeftClickInput(InputType::GOTO_MENU)
			.addButtonLeftListener(&_states.menu);

		tmpPos.y -= menuHeight * 1.2;
		addButton(tmpPos, tmpSize, "EXIT")
			.setKeyLeftClickInput(InputType::CANCEL)
			.addButtonLeftListener(&_states.exit);

		tmpSize.x = tmpSize.x * 1.2;
		tmpSize.y = winSz.y - tmpPos.y;
		tmpPos.x = (winSz.x / 2) - ((menuWidth * 1.2) / 2);
		tmpPos.y -= menuHeight * 0.5;
		addRect(tmpPos, tmpSize, glm::vec4(0.0, 0.0, 0.0, 0.0));

		_initBG();
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
 * @return false if there are an error in update
 */
bool	SceneVictory::update() {
	ASceneMenu::update();
	SceneGame & scGame = *reinterpret_cast<SceneGame *>(SceneManager::getScene(SceneNames::GAME));

	if (_states.nextLevel) {
		_states.nextLevel = false;
		if (scGame.level + 1 < static_cast<int32_t>(scGame.getNbLevel())) {
			scGame.loadLevel(scGame.level + 1);  // reload the current level
			SceneManager::loadScene(_lastSceneName);
		}
		else {
			SceneManager::loadScene(SceneNames::MAIN_MENU);
		}
	}
	else if (_states.restart) {
		_states.restart = false;
		scGame.loadLevel(scGame.level);  // reload the current level
		SceneManager::loadScene(_lastSceneName);
	}
	else if (_states.menu) {
		_states.menu = false;
		SceneManager::loadScene(SceneNames::MAIN_MENU);
	}
	else if (_states.exit) {
		_states.exit = false;
		SceneManager::loadScene(SceneNames::EXIT);
	}
	return true;
}

/**
 * @brief called when the scene is loaded
 */
void SceneVictory::load() {
	ASceneMenu::load();
	if (SceneManager::getSceneName() != SceneNames::EXIT) {
		_lastSceneName = SceneManager::getSceneName();
	}
}
