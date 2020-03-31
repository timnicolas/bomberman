#include "ScenePause.hpp"
#include "SceneGame.hpp"

ScenePause::ScenePause(Gui * gui, float const &dtTime)
: ASceneMenu(gui, dtTime),
  _lastSceneName(SceneNames::MAIN_MENU)
{}

ScenePause::ScenePause(ScenePause const & src)
: ASceneMenu(src)
{
	*this = src;
}

ScenePause::~ScenePause() {}

ScenePause & ScenePause::operator=(ScenePause const & rhs) {
	if (this != &rhs) {
		logWarn("you are copying ScenePause")
	}
	return *this;
}

/**
 * @brief init the menu
 *
 * @return true if the init succeed
 * @return false if the init failed
 */
bool			ScenePause::init() {
	glm::vec2 winSz = _gui->gameInfo.windowSize;
	glm::vec2 tmpPos;
	glm::vec2 tmpSize;
	float menuWidth = winSz.x / 2;
	float menuHeight = winSz.y / 14;

	try {
		tmpPos.x = (winSz.x / 2) - (menuWidth / 2);
		tmpPos.y = winSz.y - menuHeight * 2;
		tmpSize.x = menuWidth;
		tmpSize.y = menuHeight;
		addTitle(tmpPos, tmpSize, "Paused");

		tmpPos.y -= menuHeight * 1.8;
		addButton(tmpPos, tmpSize, "resume")
			.setKeyLeftClickInput(InputType::CONFIRM)
			.addButtonLeftListener(&_states.resume);

		tmpPos.y -= menuHeight * 1.3;
		addButton(tmpPos, tmpSize, "restart")
			.addButtonLeftListener(&_states.restart);

		tmpPos.y -= menuHeight * 1.3;
		addButton(tmpPos, tmpSize, "main   menu")
			.setKeyLeftClickInput(InputType::GOTO_MENU)
			.addButtonLeftListener(&_states.menu);

		tmpPos.y -= menuHeight * 1.3;
		addButton(tmpPos, tmpSize, "exit")
			.setKeyLeftClickInput(InputType::CANCEL)
			.addButtonLeftListener(&_states.exit);

		tmpSize.x = tmpSize.x * 1.3;
		tmpSize.y = winSz.y - tmpPos.y;
		tmpPos.x = (winSz.x / 2) - ((menuWidth * 1.3) / 2);
		tmpPos.y -= menuHeight * 0.5;
		addRect(tmpPos, tmpSize);

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
bool	ScenePause::update() {
	ASceneMenu::update();
	SceneGame & scGame = *reinterpret_cast<SceneGame *>(SceneManager::getScene(SceneNames::GAME));

	if (_states.resume) {
		_states.resume = false;
		SceneManager::loadScene(_lastSceneName);
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
void ScenePause::load() {
	ASceneMenu::load();
	if (SceneManager::getSceneName() != SceneNames::EXIT) {
		_lastSceneName = SceneManager::getSceneName();
	}
}
