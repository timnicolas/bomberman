#include "SceneHelp.hpp"
#include "Bonus.hpp"

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

	try {
		addExitButton()
			.addButtonLeftListener(&_states.menu);

		tmpPos.x = (winSz.x / 2) - (menuWidth / 2);
		tmpPos.y = winSz.y - menuHeight * 2;
		tmpSize.x = menuWidth;
		tmpSize.y = menuHeight;
		addTitle(tmpPos, tmpSize, "Help");
		tmpPos.y -= 1.3 * menuHeight;

		glm::vec2 imgSize = {menuHeight * 0.7, menuHeight * 0.7};

		ABaseUI	*tmpUI;
		for (auto bonus : Bonus::description) {
			addImage(tmpPos, imgSize, Bonus::bonusTextures[bonus.first]);
			tmpUI = &addText({tmpPos.x + 35, tmpPos.y}, {menuWidth, imgSize.y}, bonus.second)
				.setTextFont("text")
				.setTextAlign(TextAlign::LEFT);
			if (bonus.first == BonusType::DETONATOR) {
				allUI.detonatorText = tmpUI;
			}
			tmpPos.y -= 0.8 * menuHeight;
		}

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
void SceneHelp::load() {
	ASceneMenu::load();
	allUI.detonatorText->setText(Bonus::description[BonusType::DETONATOR]);
}

/**
 * @brief this is the update function (called every frames)
 *
 * @return true if the update is a success
 * @return false if there are an error in update
 */
bool	SceneHelp::update() {
	ASceneMenu::update();
	if (_states.menu || Inputs::getKeyUp(InputType::CANCEL)) {
		_states.menu = false;
		SceneManager::loadScene(SceneNames::MAIN_MENU);
	}
	return true;
}
