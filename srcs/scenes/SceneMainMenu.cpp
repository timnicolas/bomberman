#include "SceneMainMenu.hpp"
#include "AudioManager.hpp"
#include "Save.hpp"

SceneMainMenu::SceneMainMenu(Gui * gui, float const &dtTime)
: ASceneMenu(gui, dtTime)
{}

SceneMainMenu::SceneMainMenu(SceneMainMenu const & src)
: ASceneMenu(src)
{
	*this = src;
}

SceneMainMenu::~SceneMainMenu() {}

SceneMainMenu & SceneMainMenu::operator=(SceneMainMenu const & rhs) {
	if (this != &rhs) {
		logWarn("you are copying SceneMainMenu")
	}
	return *this;
}

/**
 * @brief init the menu
 *
 * @return true if the init succeed
 * @return false if the init failed
 */
bool			SceneMainMenu::init() {
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
		addTitle(tmpPos, tmpSize, "Bomberman");

		tmpPos.y -= menuHeight * 1.8;
		addButton(tmpPos, tmpSize, "new   game")
			.setKeyLeftClickInput(InputType::CONFIRM)
			.addButtonLeftListener(&_states.newGame);

		tmpPos.y -= menuHeight * 1.3;
		addButton(tmpPos, tmpSize, "load   saved   game")
			.setKeyLeftClickInput(InputType::ACTION)
			.addButtonLeftListener(&_states.loadGame);

		tmpPos.y -= menuHeight * 1.3;
		addButton(tmpPos, tmpSize, "settings")
			.setKeyLeftClickScancode(SDL_SCANCODE_COMMA)
			.addButtonLeftListener(&_states.loadSettings);

		tmpPos.y -= menuHeight * 1.3;
		addButton(tmpPos, tmpSize, "exit")
			.setKeyLeftClickInput(InputType::CANCEL)
			.addButtonLeftListener(&_states.exit);

		tmpSize.x = tmpSize.x * 1.3;
		tmpSize.y = winSz.y - tmpPos.y;
		tmpPos.x = (winSz.x / 2) - ((menuWidth * 1.3) / 2);
		tmpPos.y -= menuHeight * 0.5;
		addRect(tmpPos, tmpSize);

		AudioManager::loadMusic("sounds/puzzle.ogg");
		AudioManager::playMusic("sounds/puzzle.ogg", 1.0f, true);

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
bool	SceneMainMenu::update() {
	ASceneMenu::update();

	if (_states.newGame) {
		_states.newGame = false;
		Save::newGame();
		SceneManager::loadScene(SceneNames::LEVEL_SELECTION);
	}
	else if (_states.loadGame) {
		_states.loadGame = false;
		SceneManager::loadScene(SceneNames::LOADGAME);
	}
	else if (_states.loadSettings) {
		_states.loadSettings = false;
		SceneManager::loadScene(SceneNames::SETTINGS);
	}
	else if (_states.exit) {
		_states.exit = false;
		SceneManager::loadScene(SceneNames::EXIT);
	}
	return true;
}
