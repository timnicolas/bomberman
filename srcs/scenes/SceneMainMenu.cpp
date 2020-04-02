#include "SceneMainMenu.hpp"
#include "AudioManager.hpp"

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
	float menuHeight = menuWidth / 8;

	try {
		tmpPos.x = (winSz.x / 2) - (menuWidth / 2);
		tmpPos.y = winSz.y - menuHeight * 2;
		tmpSize.x = menuWidth;
		tmpSize.y = menuHeight;
		addText(tmpPos, tmpSize, "MENU").setTextFont("title");

		tmpPos.y -= menuHeight * 1.2;
		addButton(tmpPos, tmpSize, "PLAY")
			.setKeyLeftClickInput(InputType::CONFIRM)
			.addButtonLeftListener(&_states.play);

		tmpPos.y -= menuHeight * 1.2;
		addButton(tmpPos, tmpSize, "SETTINGS")
			.setKeyLeftClickScancode(SDL_SCANCODE_COMMA)
			.addButtonLeftListener(&_states.loadSettings);

		tmpPos.y -= menuHeight * 1.2;
		addButton(tmpPos, tmpSize, "EXIT")
			.setKeyLeftClickInput(InputType::CANCEL)
			.addButtonLeftListener(&_states.exit);

		tmpPos.y -= menuHeight * 1.2;
		addTextInput(tmpPos, tmpSize, "name")
			.setTextFont("cheatcode")
			.setColor(glm::vec4(0.1, 0.1, 0.1, 1));

		tmpSize.x = tmpSize.x * 1.2;
		tmpSize.y = winSz.y - tmpPos.y;
		tmpPos.x = (winSz.x / 2) - ((menuWidth * 1.2) / 2);
		tmpPos.y -= menuHeight * 0.5;
		addRect(tmpPos, tmpSize, glm::vec4(0.0, 0.0, 0.0, 0.0));

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

	if (_states.play) {
		_states.play = false;
		SceneManager::loadScene(SceneNames::LEVEL_SELECTION);
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
