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
	float menuWidth = winSz.x / 2;
	float menuHeight = winSz.y / 14;

	try {
		tmpPos.x = (winSz.x / 2) - (menuWidth / 2);
		tmpPos.y = winSz.y - menuHeight * 2;
		tmpSize.x = menuWidth;
		tmpSize.y = menuHeight;
		addTitle(tmpPos, tmpSize, "You   win   !");

		allUI.mainMenu = &addButton(VOID_SIZE, VOID_SIZE, "main   menu")
			.addButtonLeftListener(&_states.mainMenu);
		allUI.save = &addButton(VOID_SIZE, VOID_SIZE, "save")
			.setKeyLeftClickScancode(SDL_SCANCODE_S)
			.addButtonLeftListener(&_states.save);
		allUI.exit = &addButton(VOID_SIZE, VOID_SIZE, "exit")
			.setKeyLeftClickInput(InputType::CANCEL)
			.addButtonLeftListener(&_states.exit);
		allUI.border = &addRect(VOID_SIZE, VOID_SIZE)
			.setTextAlign(TextAlign::CENTER);

		allUI.text = &addText(VOID_SIZE, VOID_SIZE, "Ernest  Marin    Emilien  Baudet    Tim  Nicolas    Guilhem  Smith");
		allUI.text->setPos({winSz.x / 2 - allUI.text->getSize().x / 2, 30});

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
	_updateUI();
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

	if (_states.mainMenu) {
		_states.mainMenu = false;
		SceneManager::loadScene(SceneNames::MAIN_MENU);
	}
	else if (_states.save) {
		_states.save = false;
		Save::save();
		load();
	}
	else if (_states.exit) {
		_states.exit = false;
		SceneManager::loadScene(SceneNames::EXIT);
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

// -- Private methods ----------------------------------------------------------

/**
 * @brief Update UI objects.
 *
 */
void		SceneEndGame::_updateUI() {
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
		allUI.mainMenu->setPos(tmpPos).setSize(tmpSize).setKeyLeftClickInput(InputType::CONFIRM);
		tmpPos.y -= menuHeight * 1.3;
		allUI.save->setPos(tmpPos).setSize(tmpSize);
		if (Save::isSaved()) {
			allUI.save->setText("saved").setColor(colorise(s.j("colors").j("buttons-disable").u("color")));
		}
		else {
			allUI.save->setText("save ..").setColor(colorise(s.j("colors").j("buttons").u("color")));
		}
	}
	else {
		allUI.mainMenu->setPos(tmpPos).setSize(tmpSize).setKeyLeftClickInput(InputType::CONFIRM);
		allUI.save->setPos(VOID_SIZE).setSize(VOID_SIZE);
	}
	tmpPos.y -= menuHeight * 1.3;
	allUI.exit->setPos(tmpPos).setSize(tmpSize);
	tmpSize.x = tmpSize.x * 1.3;
	tmpSize.y = winSz.y - tmpPos.y;
	tmpPos.x = (winSz.x / 2) - ((menuWidth * 1.3) / 2);
	tmpPos.y -= menuHeight * 0.5;
	allUI.border->setPos(tmpPos).setSize(tmpSize);
}
