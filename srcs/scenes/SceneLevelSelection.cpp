#include "SceneLevelSelection.hpp"
#include "SceneGame.hpp"
#include "Save.hpp"
#include "SceneCheatCode.hpp"
#include "SceneManager.hpp"

SceneLevelSelection::SceneLevelSelection(Gui * gui, float const &dtTime)
: ASceneMenu(gui, dtTime),
  _currentLvl(-1),
  _transition(0)
{}

SceneLevelSelection::SceneLevelSelection(SceneLevelSelection const & src)
: ASceneMenu(src)
{
	*this = src;
}

SceneLevelSelection::~SceneLevelSelection() {}

SceneLevelSelection & SceneLevelSelection::operator=(SceneLevelSelection const & rhs) {
	if (this != &rhs) {
		logWarn("you are copying SceneLevelSelection")
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
bool			SceneLevelSelection::init() {
	glm::vec2 winSz = _gui->gameInfo.windowSize;
	glm::vec2 tmpPos;
	glm::vec2 tmpSize;
	float menuWidth = winSz.x / 2;
	float menuHeight = menuWidth / 8;
	SceneGame & scGame = *reinterpret_cast<SceneGame *>(SceneManager::getScene(SceneNames::GAME));

	_transition = 0;  // reset transition

	try {
		_states.nbLevel = scGame.getNbLevel();

		addExitButton()
			.addButtonLeftListener(&_states.menu);

		tmpSize.x = menuWidth;
		tmpSize.y = menuHeight * 5;
		tmpPos.x = (winSz.x / 2) - (tmpSize.x / 2);
		tmpPos.y = menuHeight;
		allUI.score = &addText(tmpPos, tmpSize, "score");
		tmpPos.y = menuHeight * 0.8 - menuHeight;
		allUI.text = &addText(tmpPos, tmpSize, "level");
		tmpPos.y = (winSz.y / 2) - (tmpSize.y / 2);
		for (uint32_t i = 0; i < _states.nbLevel; i++) {
			addButtonImage(tmpPos, tmpSize, scGame.getLevelImg(i));
			if (i == 0) {
				_states.firstLevelID = getNbUIElements() - 1;
			}
			tmpPos.x += winSz.x;
		}

		tmpPos.x = 30;
		tmpPos.y = winSz.y / 2 - menuHeight / 2;
		tmpSize.x = menuHeight;
		tmpSize.y = 0;
		addButtonImage(tmpPos, tmpSize, s.s("imgsUI") + "/prev.png")
			.setKeyLeftClickInput(InputType::LEFT)
			.addButtonLeftListener(&_states.lastLevel);

		tmpPos.x = winSz.x - 30 - tmpSize.x;
		addButtonImage(tmpPos, tmpSize, s.s("imgsUI") + "/next.png")
			.setKeyLeftClickInput(InputType::RIGHT)
			.addButtonLeftListener(&_states.nextLevel);

		tmpPos.x = (winSz.x / 2) - (menuWidth / 2);
		tmpPos.y = menuHeight * 0.8;
		tmpSize.x = menuWidth;
		tmpSize.y = menuHeight;
		addButton(tmpPos, tmpSize, "main   menu")
			.setKeyLeftClickInput(InputType::GOTO_MENU)
			.addButtonLeftListener(&_states.menu);

		tmpSize.x = tmpSize.x * 1.2;
		tmpSize.y = winSz.y - tmpPos.y;
		tmpPos.x = (winSz.x / 2) - ((menuWidth * 1.2) / 2);
		tmpPos.y -= menuHeight * 0.5;
		addRect(tmpPos, tmpSize);

		_initBG();
	}
	catch (ABaseUI::UIException const & e) {
		logErr(e.what());
		return false;
	}
	setLevel(0);
	return true;
}

/**
 * @brief this is the update function (called every frames)
 *
 * @return true if the update is a success
 * @return false if there are an error in update
 */
bool	SceneLevelSelection::update() {
	ASceneMenu::update();
	// size of the window
	glm::vec2 winSz = _gui->gameInfo.windowSize;
	// SceneGame reference
	SceneGame & scGame = *reinterpret_cast<SceneGame *>(SceneManager::getScene(SceneNames::GAME));

	allUI.text->setText(scGame.getLevelName(_currentLvl));
	if (Save::isLevelDone(_currentLvl)) {
		allUI.score->setText("score: " + std::to_string(Save::getLevelScore(_currentLvl)));
	} else if (_currentLvl == 0 || Save::isLevelDone(_currentLvl - 1) || SceneCheatCode::isLevelUnlocked(_currentLvl)) {
		allUI.score->setText("play");
	} else {
		allUI.score->setText("locked");
	}

	for (uint32_t i = 0; i < _states.nbLevel; i++) {
		ABaseUI & elem = getUIElement(_states.firstLevelID + i);

		/* create a smooth transition */
		float xoffset = -(_currentLvl * winSz.x) + _transition * winSz.x;
		if (_transition > 0) {
			_transition -= TRANSITION_SPEED;
			if (_transition <= 0)
				_transition = 0;
		}
		if (_transition < 0) {
			_transition += TRANSITION_SPEED;
			if (_transition >= 0)
				_transition = 0;
		}
		elem.setPosOffset(glm::vec2(xoffset, 0));
	}
	if (_states.loadLevel) {
		_states.loadLevel = false;
		if (_transition == 0) {  // load only if the transition is over
			try {
				if (!scGame.loadLevel(_currentLvl)) {
					return false;
				}
			} catch (std::exception const &e) {
				logErr("Error: " << e.what());
				SceneCheatCode & scCheatCode = *reinterpret_cast<SceneCheatCode *>(
					SceneManager::getScene(SceneNames::CHEAT_CODE)
				);
				scCheatCode.clearAllLn();
				std::stringstream ss;
				ss << "Level " << _currentLvl << ": " << e.what();
				logErrScreen(ss.str());
				scCheatCode.unlockLevel(_currentLvl + 1);
				SceneManager::loadScene(SceneNames::LEVEL_SELECTION);
				return true;
			}
			SceneManager::loadScene(SceneNames::GAME);
			return true;
		}
	}
	if (_states.menu) {
		_states.menu = false;
		SceneManager::loadScene(SceneNames::MAIN_MENU);
	}
	else if (_states.lastLevel) {
		_states.lastLevel = false;
		setLevel(_currentLvl - 1);
	}
	else if (_states.nextLevel) {
		_states.nextLevel = false;
		setLevel(_currentLvl + 1);
	}
	return true;
}

/**
 * @brief called when the scene is loaded
 */
void SceneLevelSelection::load() {
	ASceneMenu::load();
	_transition = 1;
}

// -- Private methods ----------------------------------------------------------

/**
 * @brief set the current level in selection
 *
 * @param level the level ID
 * @param enableTransition If true, enable smooth transition btw levels
 */
void			SceneLevelSelection::setLevel(int32_t level, bool enableTransition) {
	// set right level ID
	if (level < 0) level = 0;
	if (level >= static_cast<int32_t>(_states.nbLevel)) level = _states.nbLevel - 1;
	if (level == _currentLvl)
		return;

	// set transition
	if (enableTransition) {
		if (level == _currentLvl - 1) _transition = -1;
		else if (level == _currentLvl + 1) _transition = 1;
		else _transition = 0;
	}

	if (_currentLvl >= 0 && _currentLvl < static_cast<int32_t>(_states.nbLevel)) {
		getUIElement(_states.firstLevelID + _currentLvl)
			.setKeyLeftClickInput(InputType::NO_KEY)
			.addButtonLeftListener(nullptr);
	}

	_currentLvl = level;
	if (_currentLvl == 0 || Save::isLevelDone(_currentLvl) || Save::isLevelDone(_currentLvl - 1)
	|| SceneCheatCode::isLevelUnlocked(_currentLvl)) {
		getUIElement(_states.firstLevelID + _currentLvl)
			.setKeyLeftClickInput(InputType::CONFIRM)
			.addButtonLeftListener(&_states.loadLevel)
			.setColor(colorise(0, 0))
			.setMouseHoverColor(colorise(0x000000, 51))
			.setMouseClickColor(UI_DEF_MOUSE_CLICK_COLOR);
	} else {
		getUIElement(_states.firstLevelID + _currentLvl)
			.setColor(colorise(0x000000, 200))
			.setMouseHoverColor(colorise(0x000000, 200))
			.setMouseClickColor(colorise(0x000000, 200));
	}
}

/**
 * @brief Get the current level ID
 *
 * @return uint32_t The current level ID
 */
uint32_t SceneLevelSelection::getCurLevel() const { return _currentLvl; }
/**
 * @brief Get the total number of levels
 *
 * @return uint32_t the total number of levels
 */
uint32_t SceneLevelSelection::getNbLevel() const { return _states.nbLevel; }
