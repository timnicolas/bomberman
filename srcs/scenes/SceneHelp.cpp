#include "SceneHelp.hpp"
#include "Bonus.hpp"
#include "SceneLevelSelection.hpp"

/**
 * @brief Construct a new Scene Help:: Scene Help object
 *
 * @param gui A pointer on the gui object
 * @param dtTime A reference to the delta time
 */
SceneHelp::SceneHelp(Gui * gui, float const &dtTime)
: ASceneMenu(gui, dtTime)
{}

/**
 * @brief Construct a new Scene Help:: Scene Help object
 *
 * @param src A SceneHelp element to copy
 */
SceneHelp::SceneHelp(SceneHelp const & src)
: ASceneMenu(src)
{
	*this = src;
}

/**
 * @brief Destroy the Scene Help:: Scene Help object
 */
SceneHelp::~SceneHelp() {}

/**
 * @brief Overload operator =
 *
 * @param rhs Right element
 * @return SceneHelp&
 */
SceneHelp & SceneHelp::operator=(SceneHelp const & rhs) {
	if (this != &rhs) {
		logWarn("you are copying SceneHelp")
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
bool			SceneHelp::init() {
	glm::vec2 winSz = _gui->gameInfo.windowSize;
	glm::vec2 tmpPos;
	glm::vec2 tmpSize;
	float menuWidth = winSz.x * 0.6;
	float menuHeight = winSz.y / 14;

	_currentPage = 0;
	_transition = 0;  // reset transition

	try {
		addExitButton()
			.setKeyLeftClickScancode(SDL_SCANCODE_ESCAPE)
			.addButtonLeftListener(&_states.exit);

		tmpPos.x = (winSz.x / 2) - (menuWidth / 2);
		tmpPos.y = winSz.y - menuHeight * 2;
		tmpSize.x = menuWidth;
		tmpSize.y = menuHeight;
		addTitle(tmpPos, tmpSize, "Help");
		tmpPos.y -= 1.3 * menuHeight;

		glm::vec2 imgSize = {menuHeight * 0.7, menuHeight * 0.7};

		/* create all pages */
		ABaseUI	*tmpUI = nullptr;
		int pageID = 0;
		glm::ivec2 startPos = {
			(winSz.x / 2) - (menuWidth * 1.2 / 2),
			tmpPos.y
		};

		/* page 1 -> game goal */
		tmpPos = startPos;
		tmpPos.x = winSz.x / 2 + pageID * winSz.x;
		allUI.allPages.push_back(std::vector< ABaseUI * >());

		tmpUI = &addText(tmpPos, {0, imgSize.y},
			"Goal:")
			.setTextFont("title")
			.setTextScale(0.7)
			.setTextAlign(TextAlign::CENTER);
		allUI.allPages[pageID].push_back(tmpUI);
		tmpPos.y -= 1 * menuHeight;

		tmpUI = &addText(tmpPos, {0, imgSize.y},
			"Destroy the computers to unlock the exit door.")
			.setTextFont("text")
			.setTextScale(HELP_MENU_TEXT_SCALE)
			.setTextAlign(TextAlign::CENTER);
		allUI.allPages[pageID].push_back(tmpUI);
		tmpPos.y -= 0.8 * menuHeight;

		tmpUI = &addText(tmpPos, {0, imgSize.y},
			"Be careful of enemies who will obstruct your way.")
			.setTextFont("text")
			.setTextScale(HELP_MENU_TEXT_SCALE)
			.setTextAlign(TextAlign::CENTER);
		allUI.allPages[pageID].push_back(tmpUI);
		tmpPos.y -= 0.8 * menuHeight;
		tmpPos.y -= 0.8 * menuHeight;

		tmpUI = &addText(tmpPos, {0, imgSize.y},
			"Gameplay:")
			.setTextFont("title")
			.setTextScale(0.7)
			.setTextAlign(TextAlign::CENTER);
		allUI.allPages[pageID].push_back(tmpUI);
		tmpPos.y -= 1 * menuHeight;

		tmpUI = &addText(tmpPos, {0, imgSize.y},
			"[MOVING]")
			.setTextFont("text")
			.setTextScale(HELP_MENU_TEXT_SCALE)
			.setTextAlign(TextAlign::CENTER);
		allUI.movingText = tmpUI;
		allUI.allPages[pageID].push_back(tmpUI);
		tmpPos.y -= 0.8 * menuHeight;

		tmpUI = &addText(tmpPos, {0, imgSize.y},
			"[BOMB]")
			.setTextFont("text")
			.setTextScale(HELP_MENU_TEXT_SCALE)
			.setTextAlign(TextAlign::CENTER);
		allUI.bombText = tmpUI;
		allUI.allPages[pageID].push_back(tmpUI);
		tmpPos.y -= 0.8 * menuHeight;

		tmpUI = &addText(tmpPos, {0, imgSize.y},
			"[BOMB-DETONATOR]")
			.setTextFont("text")
			.setTextScale(HELP_MENU_TEXT_SCALE)
			.setTextAlign(TextAlign::CENTER);
		allUI.bombDetonatorText = tmpUI;
		allUI.allPages[pageID].push_back(tmpUI);
		tmpPos.y -= 0.8 * menuHeight;

		pageID++;

		/* page 2 -> bonus descriptions */
		tmpPos = startPos;
		tmpPos.x += pageID * winSz.x;
		allUI.allPages.push_back(std::vector< ABaseUI * >());
		for (auto bonus : Bonus::description) {
			tmpUI = &addImage(tmpPos, imgSize, Bonus::bonusTextures[bonus.first]);
			allUI.allPages[pageID].push_back(tmpUI);
			tmpUI = &addText({tmpPos.x + 35, tmpPos.y}, {menuWidth, imgSize.y}, bonus.second)
				.setTextFont("text")
				.setTextScale(HELP_MENU_TEXT_SCALE)
				.setTextAlign(TextAlign::LEFT);
			if (bonus.first == BonusType::DETONATOR) {
				allUI.detonatorText = tmpUI;
			}
			allUI.allPages[pageID].push_back(tmpUI);
			tmpPos.y -= 0.8 * menuHeight;
		}
		pageID++;

		_nbPages = pageID;

		/* buttons to switch btw pages */
		tmpPos.x = 30;
		tmpPos.y = winSz.y / 2 - menuHeight / 2;
		tmpSize.x = menuHeight;
		tmpSize.y = 0;
		allUI.leftPage = &addButtonImage(tmpPos, tmpSize, s.s("imgsUI") + "/prev.png")
			.setKeyLeftClickInput(InputType::LEFT)
			.addButtonLeftListener(&_states.leftMenu);

		tmpPos.x = winSz.x - 30 - tmpSize.x;
		allUI.rightPage = &addButtonImage(tmpPos, tmpSize, s.s("imgsUI") + "/next.png")
			.setKeyLeftClickInput(InputType::RIGHT)
			.addButtonLeftListener(&_states.rightMenu);

		/* green bg */
		tmpPos.x = (winSz.x / 2) - ((menuWidth * 1.3) / 2);
		tmpPos.y = menuHeight * 2;
		tmpSize.x = winSz.x - 2 * tmpPos.x;
		tmpSize.y = winSz.y - tmpPos.y;
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
void SceneHelp::load() {
	ASceneMenu::load();
	allUI.detonatorText->setText(Bonus::description[BonusType::DETONATOR]);
	allUI.movingText->setText("Move with: "
		+ Inputs::getKeyName(InputType::UP) + ", "
		+ Inputs::getKeyName(InputType::DOWN) + ", "
		+ Inputs::getKeyName(InputType::LEFT) + ", "
		+ Inputs::getKeyName(InputType::RIGHT));
	allUI.bombText->setText("You can drop bombs with " + Inputs::getKeyName(InputType::ACTION));
	allUI.bombDetonatorText->setText("If you have the detonator bonus, explode bombs with "
		+ Inputs::getKeyName(InputType::ACTION_2));
	if (SceneManager::getSceneName() != SceneNames::EXIT) {
		_lastSceneName = SceneManager::getSceneName();
		if (_lastSceneName == SceneNames::PAUSE) {
			_draw3dMenu = false;
		}
	}
}

/**
 * @brief this is the update function (called every frames)
 *
 * @return true if the update is a success
 * @return false if there are an error in update
 */
bool	SceneHelp::update() {
	ASceneMenu::update();
	glm::vec2 winSz = _gui->gameInfo.windowSize;

	if (_states.exit || Inputs::getKeyUp(InputType::CONFIRM) || Inputs::getKeyUp(InputType::ACTION)) {
		_states.exit = false;
		SceneManager::loadScene(_lastSceneName);
	}
	else if (_states.leftMenu) {
		_states.leftMenu = false;
		setPage(_currentPage - 1);
	}
	else if (_states.rightMenu) {
		_states.rightMenu = false;
		setPage(_currentPage + 1);
	}

	/* draw the right page */
	for (uint32_t i = 0; i < _nbPages; i++) {
		/* create a smooth transition */
		float xoffset = -(_currentPage * winSz.x) + _transition * winSz.x;
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
		for (auto && elem : allUI.allPages[i]) {
			elem->setPosOffset(glm::vec2(xoffset, 0));
		}
	}

	/* disable arrow buttons if first or last level */
	allUI.leftPage->setEnabled(_currentPage != 0);
	allUI.rightPage->setEnabled(_currentPage < static_cast<int32_t>(_nbPages - 1));

	return true;
}


/**
 * @brief set the current level in selection
 *
 * @param page the level ID
 * @param enableTransition If true, enable smooth transition btw levels
 */
void	SceneHelp::setPage(int32_t page, bool enableTransition) {
	// set right page ID
	if (page < 0) page = 0;
	if (page >= static_cast<int32_t>(_nbPages)) page = _nbPages - 1;
	if (page == _currentPage)
		return;

	// set transition
	if (enableTransition) {
		if (page == _currentPage - 1) _transition = -1;
		else if (page == _currentPage + 1) _transition = 1;
		else _transition = 0;
	}

	_currentPage = page;
}
