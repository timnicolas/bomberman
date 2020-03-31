#include "SceneExit.hpp"

SceneExit::SceneExit(Gui * gui, float const &dtTime)
: ASceneMenu(gui, dtTime),
  _lastSceneName(SceneNames::MAIN_MENU)
{}

SceneExit::SceneExit(SceneExit const & src)
: ASceneMenu(src)
{
	*this = src;
}

SceneExit::~SceneExit() {}

SceneExit & SceneExit::operator=(SceneExit const & rhs) {
	if (this != &rhs) {
		logWarn("you are copying SceneExit")
	}
	return *this;
}

/**
 * @brief init the menu
 *
 * @return true if the init succeed
 * @return false if the init failed
 */
bool			SceneExit::init() {
	glm::vec2 winSz = _gui->gameInfo.windowSize;
	glm::vec2 tmpPos;
	glm::vec2 tmpSize;
	float menuWidth = winSz.x / 2;
	float menuHeight = winSz.y / 14;

	try {
		tmpPos.x = (winSz.x / 2) - (menuWidth / 2);
		// tmpPos.x = (winSz.x / 2) - ((menuWidth * 1.2) / 2);
		tmpPos.y = winSz.y - menuHeight * 2;
		tmpSize.x = menuWidth;
		tmpSize.y = menuHeight;
		addTitle(tmpPos, tmpSize, "Do   you   want   to   exit ?");

		tmpPos.y -= menuHeight * 1.8;
		addButton(tmpPos, tmpSize, "exit")
			.setKeyLeftClickInput(InputType::CONFIRM)
			.addButtonLeftListener(&_states.exit);

		tmpPos.y -= menuHeight * 1.3;
		addButton(tmpPos, tmpSize, "cancel")
			.setKeyLeftClickInput(InputType::CANCEL)
			.addButtonLeftListener(&_states.cancel);

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
bool	SceneExit::update() {
	ASceneMenu::update();

	if (_states.exit) {
		_states.exit = false;
		SceneManager::quit();
	}
	else if (_states.cancel) {
		_states.cancel = false;
		SceneManager::loadScene(_lastSceneName);
	}
	return true;
}

/**
 * @brief called when the scene is loaded
 */
void SceneExit::load() {
	ASceneMenu::load();
	if (SceneManager::getSceneName() != SceneNames::EXIT) {
		_lastSceneName = SceneManager::getSceneName();
	}
}
