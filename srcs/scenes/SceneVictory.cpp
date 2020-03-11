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
			.addButtonLeftListener(&_states.nextLevel);

		tmpPos.y -= menuHeight * 1.2;
		addButton(tmpPos, tmpSize, "RESTART")
			.addButtonLeftListener(&_states.restart);

		tmpPos.y -= menuHeight * 1.2;
		addButton(tmpPos, tmpSize, "MAIN MENU")
			.addButtonLeftListener(&_states.menu);

		tmpPos.y -= menuHeight * 1.2;
		addButton(tmpPos, tmpSize, "EXIT")
			.addButtonLeftListener(&_states.exit);

		tmpSize.x = tmpSize.x * 1.2;
		tmpSize.y = winSz.y - tmpPos.y;
		tmpPos.x = (winSz.x / 2) - ((menuWidth * 1.2) / 2);
		tmpPos.y -= menuHeight * 0.5;
		addRect(tmpPos, tmpSize, glm::vec4(0.0, 0.0, 0.0, 0.0));

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
bool	SceneVictory::update() {
	ASceneMenu::update();
	SceneGame & scGame = *reinterpret_cast<SceneGame *>(SceneManager::getScene(SceneNames::GAME));

	if (_states.nextLevel) {
		if (scGame.level + 1 < static_cast<int32_t>(scGame.getNbLevel())) {
			scGame.loadLevel(scGame.level + 1);  // reload the current level
			SceneManager::loadScene(_lastSceneName);
		}
		else {
			SceneManager::loadScene(SceneNames::MAIN_MENU);
		}
		_states.nextLevel = false;
	}
	else if (_states.restart) {
		scGame.loadLevel(scGame.level);  // reload the current level
		SceneManager::loadScene(_lastSceneName);
		_states.restart = false;
	}
	else if (_states.menu) {
		SceneManager::loadScene(SceneNames::MAIN_MENU);
		_states.menu = false;
	}
	else if (_states.exit) {
		SceneManager::loadScene(SceneNames::EXIT);
		_states.exit = false;
	}
	return true;
}

/**
 * @brief called when the scene is loaded
 *
 */
void SceneVictory::load() {
	ASceneMenu::load();
	if (SceneManager::getSceneName() != SceneNames::EXIT) {
		_lastSceneName = SceneManager::getSceneName();
	}
}

bool SceneVictory::_initBG() {
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
