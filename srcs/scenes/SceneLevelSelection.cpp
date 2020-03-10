#include "SceneLevelSelection.hpp"
#include "SceneGame.hpp"

SceneLevelSelection::SceneLevelSelection(Gui * gui, float const &dtTime)
: ASceneMenu(gui, dtTime),
  _currentLvl(0)
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

	try {
		_states.nbLevel = scGame.getNbLevel();
		tmpSize.x = menuWidth;
		tmpSize.y = menuHeight * 5;
		tmpPos.x = (winSz.x / 2) - (menuWidth / 2);
		tmpPos.y = winSz.y - tmpSize.y * 1.2;
		for (uint32_t i = 0; i < _states.nbLevel; i++) {
			addButton(tmpPos, tmpSize, scGame.getLevelName(i)).setTextAlign(TextAlign::CENTER);
			if (i == 0) {
				_states.firstLevelID = getNbUIElements() - 1;
			tmpPos.x += winSz.x;
			}
		}

		tmpPos.x = 30;
		tmpPos.y = winSz.y / 2 - menuHeight / 2;
		tmpSize.x = menuHeight;
		tmpSize.y = menuHeight;
		addButton(tmpPos, tmpSize, "<").setTextFont("title")
			.addButtonLeftListener(&_states.lastLevel);

		tmpPos.x = winSz.x - 30 - tmpSize.x;
		addButton(tmpPos, tmpSize, ">").setTextFont("title")
			.addButtonLeftListener(&_states.nextLevel);

		tmpPos.x = (winSz.x / 2) - (menuWidth / 2);
		tmpPos.y = menuHeight * 0.8;
		tmpSize.x = menuWidth;
		tmpSize.y = menuHeight;
		addButton(tmpPos, tmpSize, "MAIN MENU").setTextAlign(TextAlign::CENTER)
			.addButtonLeftListener(&_states.menu);

		_initBG();
	}
	catch (ABaseUI::UIException & e) {
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
bool	SceneLevelSelection::update() {
	ASceneMenu::update();
	// size of the window
	glm::vec2 winSz = _gui->gameInfo.windowSize;
	// SceneGame reference
	SceneGame & scGame = *reinterpret_cast<SceneGame *>(SceneManager::getScene(SceneNames::GAME));

	for (uint32_t i = 0; i < _states.nbLevel; i++) {
		ABaseUI & elem = getUIElement(_states.firstLevelID + i);
		elem.setPosOffset(glm::vec2(-(_currentLvl * winSz.x), 0));
		if (elem.getMouseLeftClick()) {
			scGame.loadLevel(i);
			SceneManager::loadScene(SceneNames::GAME);
			return true;
		}
	}
	if (_states.menu) {
		SceneManager::loadScene(SceneNames::MAIN_MENU);
		_states.menu = false;
	}
	else if (_states.lastLevel) {
		if (_currentLvl > 0) {
			_currentLvl--;
		}
		_states.lastLevel = false;
	}
	else if (_states.nextLevel) {
		if (_currentLvl < _states.nbLevel - 1) {
			_currentLvl++;
		}
		_states.nextLevel = false;
	}
	return true;
}

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
			if ((i + j) & 1)	name = "bomberman-assets/textures/bomb/005-bombFace.png";
			else		name = "bomberman-assets/textures/player/009-playerFace.png";
			addImage(tmpPos, tmpSize, name, false).setColor(glm::vec4(1.0, 1.0, 1.0, 0.5));
			tmpPos.x += tmpSize.x;
			j++;
		}
		tmpPos.y += getUIElement(getNbUIElements() - 1).getSize().y;
		i++;
	}
	return true;
}
