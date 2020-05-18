#include "SceneExit.hpp"
#include "Save.hpp"

/**
 * @brief Construct a new Scene Exit:: Scene Exit object
 *
 * @param gui A pointer on the gui object
 * @param dtTime A reference to the delta time
 */
SceneExit::SceneExit(Gui * gui, float const &dtTime)
: ASceneMenu(gui, dtTime),
  _lastSceneName(SceneNames::MAIN_MENU)
{}

/**
 * @brief Construct a new Scene Exit:: Scene Exit object
 *
 * @param src The object to do the copy
 */
SceneExit::SceneExit(SceneExit const & src)
: ASceneMenu(src)
{
	*this = src;
}

/**
 * @brief Destroy the Scene Exit:: Scene Exit object
 */
SceneExit::~SceneExit() {}

/**
 * @brief Copy this object
 *
 * @param rhs The object to copy
 * @return SceneExit& A reference to the copied object
 */
SceneExit & SceneExit::operator=(SceneExit const & rhs) {
	if (this != &rhs) {
		logWarn("you are copying SceneExit")
	}
	return *this;
}

// - AScene Public Methods -----------------------------------------------------

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


/**
 * @brief this is the update function (called every frames)
 *
 * @return true if the update is a success
 * @return false if there are an error in update
 */
bool	SceneExit::update() {
	_updateUI();
	ASceneMenu::update();

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
		if (_lastSceneName == SceneNames::PAUSE) {
			_draw3dMenu = false;
		}
	}
}

/**
 * @brief called when the scene is unloaded
 */
void SceneExit::unload() {
	ASceneMenu::unload();
	if (_lastSceneName == SceneNames::PAUSE) {
		_draw3dMenu = true;
	}
}

/**
 * @brief this is the draw function (called every frames)
 *
 * @return true if the draw is a success
 * @return false if there are an error in draw
 */
bool SceneExit::draw() {
	bool ret = true;

	/* 3d background */
	if (_draw3dMenu == false && s.j("debug").b("3d-menu")) {
		SceneGame & scGame = *reinterpret_cast<SceneGame *>(SceneManager::getScene(SceneNames::GAME));
		ret = scGame.draw();  // draw the game if possible
	}
	ret = ASceneMenu::draw();
	return ret & true;
}


// -- Private methods ----------------------------------------------------------

/**
 * @brief Update UI objects.
 *
 */
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
