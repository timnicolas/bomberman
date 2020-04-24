#include "SceneEndGame.hpp"
#include "Save.hpp"

SceneEndGame::SceneEndGame(Gui * gui, float const &dtTime)
: ASceneMenu(gui, dtTime)
{
	_draw3dMenu = false;
}

SceneEndGame::SceneEndGame(SceneEndGame const & src)
: ASceneMenu(src)
{
	*this = src;
}

SceneEndGame::~SceneEndGame() {}

SceneEndGame & SceneEndGame::operator=(SceneEndGame const & rhs) {
	if (this != &rhs) {
		logWarn("you are copying SceneEndGame")
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
bool			SceneEndGame::init() {
	glm::vec2 winSz = _gui->gameInfo.windowSize;
	glm::vec2 tmpPos;
	glm::vec2 tmpSize;
	float menuWidth = winSz.x * 0.8;
	float menuHeight = winSz.y / 14;

	try {
		tmpSize.y = winSz.y * 0.08;
		tmpSize.x = tmpSize.y;
		tmpPos.x = tmpSize.x / 2;
		tmpPos.y = winSz.y - tmpSize.y * 1.5;
		addButton(tmpPos, tmpSize, "X")
			.addButtonLeftListener(&_states.exit)
			.setKeyLeftClickInput(InputType::CANCEL)
			.setTextAlign(TextAlign::CENTER);

		tmpPos.x = (winSz.x / 2) - (menuWidth / 2);
		tmpPos.y = winSz.y - menuHeight * 2;
		tmpSize.x = menuWidth;
		tmpSize.y = menuHeight;
		addTitle(tmpPos, tmpSize, "Bomberman");

		tmpPos.x = winSz.x / 2 - menuWidth / 2;
		tmpPos.y -= menuHeight * 1.3;
		addText(tmpPos, {menuWidth, menuHeight}, "Thank's you for playing !")
			.setTextAlign(TextAlign::CENTER);

		tmpPos.x = winSz.x / 2 - menuWidth / 2;
		tmpPos.y -= menuHeight * 1.3;
		addText(tmpPos, {menuWidth, menuHeight}, "Ernest  Marin")
			.setTextAlign(TextAlign::CENTER);

		tmpPos.x = winSz.x / 2 - menuWidth / 2;
		tmpPos.y -= menuHeight;
		addText(tmpPos, {menuWidth, menuHeight}, "Emilien  Baudet")
			.setTextAlign(TextAlign::CENTER);

		tmpPos.x = winSz.x / 2 - menuWidth / 2;
		tmpPos.y -= menuHeight;
		addText(tmpPos, {menuWidth, menuHeight}, "Tim  Nicolas")
			.setTextAlign(TextAlign::CENTER);

		tmpPos.x = winSz.x / 2 - menuWidth / 2;
		tmpPos.y -= menuHeight;
		addText(tmpPos, {menuWidth, menuHeight}, "Guilhem  Smith")
			.setTextAlign(TextAlign::CENTER);

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
void SceneEndGame::load() {
	ASceneMenu::load();
}

/**
 * @brief this is the update function (called every frames)
 *
 * @return true if the update is a success
 * @return false if there are an error in update
 */
bool	SceneEndGame::update() {
	ASceneMenu::update();
	SceneGame & scGame = *reinterpret_cast<SceneGame *>(SceneManager::getScene(SceneNames::GAME));
	if (s.j("debug").b("3d-menu")) {
		scGame.updateForMenu();
	}

	if (_states.exit) {
		_states.exit = false;
		SceneManager::loadScene(SceneNames::MAIN_MENU);
	}
	return true;
}

/**
 * @brief this is the draw function (called every frames)
 *
 * @return true if the draw is a success
 * @return false if there are an error in draw
 */
bool SceneEndGame::draw() {
	bool ret = true;

	/* 3d background */
	if (s.j("debug").b("3d-menu")) {
		SceneGame & scGame = *reinterpret_cast<SceneGame *>(SceneManager::getScene(SceneNames::GAME));
		ret = scGame.drawEndGame();  // draw the game if possible
	}
	ret = ASceneMenu::draw();
	return ret & true;
}
