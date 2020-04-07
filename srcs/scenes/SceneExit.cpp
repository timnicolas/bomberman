#include "SceneExit.hpp"
#include "Save.hpp"

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

		allUI.exit = &addButton(VOID_SIZE, VOID_SIZE, "exit")
			.addButtonLeftListener(&_states.exit);
		allUI.cancel = &addButton(VOID_SIZE, VOID_SIZE, "cancel")
			.setKeyLeftClickInput(InputType::CANCEL)
			.addButtonLeftListener(&_states.cancel);
		allUI.save = &addButton(VOID_SIZE, VOID_SIZE, "save   and   exit")
			.addButtonLeftListener(&_states.save);
		allUI.border = &addRect(VOID_SIZE, VOID_SIZE);

		_initBG();
	}
	catch (ABaseUI::UIException const & e) {
		logErr(e.what());
		return false;
	}
	return true;
}
void	SceneExit::_updateUI() {
	glm::vec2 winSz = _gui->gameInfo.windowSize;
	glm::vec2 tmpPos;
	glm::vec2 tmpSize;
	float menuWidth = winSz.x / 2;
	float menuHeight = winSz.y / 14;

	tmpPos.x = (winSz.x / 2) - (menuWidth / 2);
	// tmpPos.x = (winSz.x / 2) - ((menuWidth * 1.2) / 2);
	tmpPos.y = winSz.y - menuHeight * 2;
	tmpSize.x = menuWidth;
	tmpSize.y = menuHeight;

	tmpPos.y -= menuHeight * 1.8;
	allUI.exit->setPos(tmpPos).setSize(tmpSize);
	if (!Save::isInstantiate() || (Save::isInstantiate() && Save::isSaved())) {
		allUI.exit->setKeyLeftClickInput(InputType::CONFIRM);
	} else {
		allUI.exit->setKeyLeftClickInput(InputType::ACTION);
	}
	tmpPos.y -= menuHeight * 1.3;
	allUI.cancel->setPos(tmpPos).setSize(tmpSize);
	if (!Save::isInstantiate() || (Save::isInstantiate() && Save::isSaved())) {
		allUI.save->setPos(VOID_SIZE).setSize(VOID_SIZE).setKeyLeftClickInput(InputType::NO_KEY);
	} else {
		tmpPos.y -= menuHeight * 1.3;
		allUI.save->setPos(tmpPos).setSize(tmpSize).setKeyLeftClickInput(InputType::CONFIRM);
	}
	tmpSize.x = tmpSize.x * 1.3;
	tmpSize.y = winSz.y - tmpPos.y;
	tmpPos.x = (winSz.x / 2) - ((menuWidth * 1.3) / 2);
	tmpPos.y -= menuHeight * 0.5;
	allUI.border->setPos(tmpPos).setSize(tmpSize);
}

/**
 * @brief this is the update function (called every frames)
 *
 * @return true if the update is a success
 * @return false if there are an error in update
 */
bool	SceneExit::update() {
	ASceneMenu::update();
	_updateUI();

	if (_states.exit) {
		_states.exit = false;
		if (Save::isInstantiate()) {
			Save::deleteTemp();
		}
		SceneManager::quit();
	}
	else if (_states.cancel) {
		_states.cancel = false;
		SceneManager::loadScene(_lastSceneName);
	}
	else if (_states.save) {
		_states.save = false;
		Save::save();
		SceneManager::quit();
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


