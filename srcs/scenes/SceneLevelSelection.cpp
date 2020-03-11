#include "SceneLevelSelection.hpp"
#include "SceneGame.hpp"

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

/**
 * @brief init the menu
 *
 * @return true if the init succed
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
		tmpSize.x = menuWidth;
		tmpSize.y = menuHeight * 5;
		tmpPos.x = (winSz.x / 2) - (tmpSize.x / 2);
		tmpPos.y = (winSz.y / 2) - (tmpSize.y / 2);
		for (uint32_t i = 0; i < _states.nbLevel; i++) {
			addButtonImage(tmpPos, tmpSize, scGame.getLevelImg(i), false);
			if (i == 0) {
				_states.firstLevelID = getNbUIElements() - 1;
			}
			tmpPos.x += winSz.x;
		}

		tmpPos.x = 30;
		tmpPos.y = winSz.y / 2 - menuHeight / 2;
		tmpSize.x = menuHeight;
		tmpSize.y = menuHeight;
		addButton(tmpPos, tmpSize, "<").setTextFont("title")
			.setKeyLeftClickInput(InputType::LEFT)
			.addButtonLeftListener(&_states.lastLevel);

		tmpPos.x = winSz.x - 30 - tmpSize.x;
		addButton(tmpPos, tmpSize, ">").setTextFont("title")
			.setKeyLeftClickInput(InputType::RIGHT)
			.addButtonLeftListener(&_states.nextLevel);

		tmpPos.x = (winSz.x / 2) - (menuWidth / 2);
		tmpPos.y = menuHeight * 0.8;
		tmpSize.x = menuWidth;
		tmpSize.y = menuHeight;
		addButton(tmpPos, tmpSize, "MAIN MENU")
			.addButtonLeftListener(&_states.menu);

		_initBG();
	}
	catch (ABaseUI::UIException & e) {
		logErr(e.what());
		return false;
	}
	_setLevel(0);
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
			scGame.loadLevel(_currentLvl);
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
		_setLevel(_currentLvl - 1);
	}
	else if (_states.nextLevel) {
		_states.nextLevel = false;
		_setLevel(_currentLvl + 1);
	}
	return true;
}

/**
 * @brief set the current level in selection
 *
 * @param level the level ID
 */
void			SceneLevelSelection::_setLevel(int32_t level) {
	// set right level ID
	if (level < 0) level = 0;
	if (level >= static_cast<int32_t>(_states.nbLevel)) level = _states.nbLevel - 1;
	if (level == _currentLvl)
		return;

	// set transition
	if (level == _currentLvl - 1) _transition = -1;
	else if (level == _currentLvl + 1) _transition = 1;
	else _transition = 0;

	if (_currentLvl >= 0 && _currentLvl < static_cast<int32_t>(_states.nbLevel)) {
		getUIElement(_states.firstLevelID + _currentLvl)
			.setKeyLeftClickInput(InputType::NO_KEY)
			.addButtonLeftListener(nullptr);
	}

	_currentLvl = level;

	getUIElement(_states.firstLevelID + _currentLvl)
		.setKeyLeftClickInput(InputType::CONFIRM)
		.addButtonLeftListener(&_states.loadLevel);
}

/**
 * @brief set the background
 *
 * @return true if success
 * @return false if failed
 */
bool			SceneLevelSelection::_initBG() {
	glm::vec2 winSz = _gui->gameInfo.windowSize;
	glm::vec2 tmpPos = glm::vec2(0, 0);
	glm::vec2 tmpSize = glm::vec2(200, 0);
	int i = 0;
	while (tmpPos.y < winSz.y) {
		tmpPos.x = 0;
		int j = 0;
		while (tmpPos.x < winSz.x) {
			std::string name;
			if ((i + j) & 1) name = "bomberman-assets/textures/bomb/005-bombFace.png";
			else name = "bomberman-assets/textures/player/009-playerFace.png";
			addImage(tmpPos, tmpSize, name, false).setColor(glm::vec4(1.0, 1.0, 1.0, 0.5));
			tmpPos.x += tmpSize.x;
			j++;
		}
		tmpPos.y += getUIElement(getNbUIElements() - 1).getSize().y;
		i++;
	}
	return true;
}
