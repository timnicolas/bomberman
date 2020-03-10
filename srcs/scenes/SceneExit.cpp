#include "SceneExit.hpp"

SceneExit::SceneExit(Gui * gui, float const &dtTime)
: ASceneMenu(gui, dtTime)
{}

SceneExit::SceneExit(SceneExit const & src)
: ASceneMenu(src)
{
	*this = src;
}

SceneExit::~SceneExit() {}

SceneExit & SceneExit::operator=(SceneExit const & rhs) {
	if (this != &rhs) {
		logWarn("you are copying SceneExit")
	}
	return *this;
}

/**
 * @brief init the menu
 *
 * @return true if the init succed
 * @return false if the init failed
 */
bool			SceneExit::init() {
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
		addText(tmpPos, tmpSize, "Do you want to exit ?").setTextFont("title");

		tmpPos.y -= menuHeight * 1.2;
		addButton(tmpPos, tmpSize, "EXIT").setTextAlign(TextAlign::CENTER)
			.addButtonLeftListener(&_states.exit);

		tmpPos.y -= menuHeight * 1.2;
		addButton(tmpPos, tmpSize, "CANCEL").setTextAlign(TextAlign::CENTER)
			.addButtonLeftListener(&_states.cancel);

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
bool	SceneExit::update() {
	ASceneMenu::update();

	if (_states.exit) {
		SceneManager::quit();
		_states.exit = false;
	}
	else if (_states.cancel) {
		logWarn("cancel to do");
		_states.cancel = false;
	}
	return true;
}

bool			SceneExit::_initBG() {
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
