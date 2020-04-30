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

// - AScene Public Methods -----------------------------------------------------

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

		allUI.continueGame = &addButton(VOID_SIZE, VOID_SIZE, "play")
			.addButtonLeftListener(&_states.continueGame);
		allUI.save = &addButton(VOID_SIZE, VOID_SIZE, "save")
			.setKeyLeftClickScancode(SDL_SCANCODE_S)
			.addButtonLeftListener(&_states.save);
		allUI.newGame = &addButton(VOID_SIZE, VOID_SIZE, "new   game")
			.setKeyLeftClickScancode(SDL_SCANCODE_N)
			.addButtonLeftListener(&_states.newGame);
		allUI.loadGame = &addButton(VOID_SIZE, VOID_SIZE, "load   saved   game")
			.setKeyLeftClickInput(InputType::ACTION)
			.addButtonLeftListener(&_states.loadGame);
		allUI.loadSettings = &addButton(VOID_SIZE, VOID_SIZE, "settings")
			.setKeyLeftClickScancode(SDL_SCANCODE_COMMA)
			.addButtonLeftListener(&_states.loadSettings);
		allUI.exit = &addButton(VOID_SIZE, VOID_SIZE, "exit")
			.setKeyLeftClickInput(InputType::CANCEL)
			.addButtonLeftListener(&_states.exit);
		allUI.border = &addRect(VOID_SIZE, VOID_SIZE);

		AudioManager::loadMusic("sounds/puzzle.ogg");
		AudioManager::playMusic("sounds/puzzle.ogg", 0.1f, true);

		_initBG();
	}
	catch (ABaseUI::UIException const & e) {
		logErr(e.what());
		return false;
	}
	return true;
}

/**
 * @brief called when the scene is loaded
 */
void SceneMainMenu::load() {
	ASceneMenu::load();
	_updateUI();
}

/**
 * @brief this is the update function (called every frames)
 *
 * @return true if the update is a success
 * @return false if there are an error in update
 */
bool	SceneMainMenu::update() {
	ASceneMenu::update();
	if (_states.continueGame) {
		_states.continueGame = false;
		SceneManager::loadScene(SceneNames::LEVEL_SELECTION);
	}
	if (_states.save) {
		_states.save = false;
		Save::save();
		load();
	}
	if (_states.newGame) {
		_states.newGame = false;
		SceneManager::loadScene(SceneNames::DIFFICULTY);
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

// -- Private methods ----------------------------------------------------------

/**
 * @brief Update UI objects.
 *
 */
void		SceneMainMenu::_updateUI() {
	glm::vec2 winSz = _gui->gameInfo.windowSize;
	glm::vec2 tmpPos;
	glm::vec2 tmpSize;
	float menuWidth = winSz.x / 2;
	float menuHeight = winSz.y / 14;
	tmpPos.x = (winSz.x / 2) - (menuWidth / 2);
		tmpPos.y = winSz.y - menuHeight * 2;
		tmpSize.x = menuWidth;
		tmpSize.y = menuHeight;
	tmpPos.y -= menuHeight * 1.8;
	if (Save::isInstantiate()) {
		allUI.continueGame->setPos(tmpPos).setSize(tmpSize).setKeyLeftClickInput(InputType::CONFIRM);
		tmpPos.y -= menuHeight * 1.3;
		allUI.save->setPos(tmpPos).setSize(tmpSize);
		if (Save::isSaved()) {
			allUI.save->setText("saved").setColor(colorise(s.j("colors").j("buttons-disable").u("color")));
		} else {
			allUI.save->setText("save ..").setColor(colorise(s.j("colors").j("buttons").u("color")));
		}
		tmpPos.y -= menuHeight * 1.3;
		allUI.newGame->setPos(tmpPos).setSize(tmpSize).setKeyLeftClickInput(InputType::NO_KEY);
	} else {
		allUI.continueGame->setPos(VOID_SIZE).setSize(VOID_SIZE).setKeyLeftClickInput(InputType::NO_KEY);
		allUI.save->setPos(VOID_SIZE).setSize(VOID_SIZE);
		allUI.newGame->setPos(tmpPos).setSize(tmpSize).setKeyLeftClickInput(InputType::CONFIRM);
	}
	tmpPos.y -= menuHeight * 1.3;
	allUI.loadGame->setPos(tmpPos).setSize(tmpSize);
	tmpPos.y -= menuHeight * 1.3;
	allUI.loadSettings->setPos(tmpPos).setSize(tmpSize);
	tmpPos.y -= menuHeight * 1.3;
	allUI.exit->setPos(tmpPos).setSize(tmpSize);
	tmpSize.x = tmpSize.x * 1.3;
	tmpSize.y = winSz.y - tmpPos.y;
	tmpPos.x = (winSz.x / 2) - ((menuWidth * 1.3) / 2);
	tmpPos.y -= menuHeight * 0.5;
	allUI.border->setPos(tmpPos).setSize(tmpSize);
}
