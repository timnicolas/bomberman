#include "SceneLoadGame.hpp"
#include "SceneGame.hpp"

SceneLoadGame::SceneLoadGame(Gui * gui, float const &dtTime)
: ASceneMenu(gui, dtTime),
  _lastSceneName(SceneNames::MAIN_MENU)
{
	_states.restart = false;
}

SceneLoadGame::SceneLoadGame(SceneLoadGame const & src)
: ASceneMenu(src)
{
	*this = src;
}

SceneLoadGame::~SceneLoadGame() {}

SceneLoadGame & SceneLoadGame::operator=(SceneLoadGame const & rhs) {
	if (this != &rhs) {
		logWarn("you are copying SceneLoadGame")
	}
	return *this;
}

/**
 * @brief init the menu
 *
 * @return true if the init succeed
 * @return false if the init failed
 */
bool			SceneLoadGame::init() {
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
		addTitle(tmpPos, tmpSize, "Load   game");
		tmpPos.y -= menuHeight * 1.8;

		// Screen Saved Games
		glm::vec2		savedGamesSize = {menuWidth * (2.0/5.0), menuHeight * 1.3 * 5};
		glm::vec2		savedGamesPos = {tmpPos.x + 0, tmpPos.y - savedGamesSize.y};
		ABaseMasterUI	*savedGames = reinterpret_cast<ABaseMasterUI*>(
			&addScrollbar(savedGamesPos, savedGamesSize).enableVertScroll(true).setEnabled(true)
		);
		// savedGames->setEnabled(false);
		// savedGames->setColor(colorise(0x91BD55));

		for (int i = 0; i <= 10; i++) {
			addButton(
				{0, (savedGamesSize.y) - (menuHeight * i * 1.3)},
				{savedGamesSize.x * 0.8, tmpSize.y},
				"saved  " + std::to_string(i)
			).setMaster(savedGames);
		}

		// Screen Game Preview
		glm::vec2		previewSize = {menuWidth * (3.0/5.0), menuHeight * 1.3 * 5};
		glm::vec2		previewPos = {tmpPos.x + savedGamesSize.x, tmpPos.y - previewSize.y};
		ABaseMasterUI	*previewGame = &addScrollbar(previewPos, previewSize);
		// previewGame->setColor(colorise(0xEF9224));

		addButton(
			{5 * 1.3, 0},
			tmpSize,
			"load   game"
		).setMaster(previewGame);

		tmpPos.y -= savedGamesSize.y;


		// addButton(tmpPos, tmpSize, "restart")
		// 	.setKeyLeftClickInput(InputType::CONFIRM)
		// 	.addButtonLeftListener(&_states.restart).setMaster(savedGames);

		tmpPos.y -= menuHeight * 1.3;
		addButton(tmpPos, tmpSize, "main menu")
			.setKeyLeftClickInput(InputType::GOTO_MENU)
			.addButtonLeftListener(&_states.menu);

		tmpPos.y -= menuHeight * 1.3;
		addButton(tmpPos, tmpSize, "exit")
			.setKeyLeftClickInput(InputType::CANCEL)
			.addButtonLeftListener(&_states.exit);

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
 * @brief called when the scene is loaded
 */
void SceneLoadGame::load() {
	ASceneMenu::load();
	if (SceneManager::getSceneName() != SceneNames::EXIT) {
		_lastSceneName = SceneManager::getSceneName();
	}
}

/**
 * @brief this is the update function (called every frames)
 *
 * @return true if the update is a success
 * @return false if there are an error in update
 */
bool	SceneLoadGame::update() {
	ASceneMenu::update();
	SceneGame & scGame = *reinterpret_cast<SceneGame *>(SceneManager::getScene(SceneNames::GAME));

	if (_states.restart) {
		_states.restart = false;
		scGame.loadLevel(scGame.level);  // reload the current level
		SceneManager::loadScene(_lastSceneName);
	}
	else if (_states.menu) {
		_states.menu = false;
		SceneManager::loadScene(SceneNames::MAIN_MENU);
	}
	else if (_states.exit) {
		_states.exit = false;
		SceneManager::loadScene(SceneNames::EXIT);
	}
	return true;
}
