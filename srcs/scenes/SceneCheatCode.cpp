#include "SceneCheatCode.hpp"
#include "SceneGame.hpp"

SceneCheatCode::SceneCheatCode(Gui * gui, float const &dtTime)
: ASceneMenu(gui, dtTime)
{}

SceneCheatCode::SceneCheatCode(SceneCheatCode const & src)
: ASceneMenu(src)
{
	*this = src;
}

SceneCheatCode::~SceneCheatCode() {}

SceneCheatCode & SceneCheatCode::operator=(SceneCheatCode const & rhs) {
	if (this != &rhs) {
		logWarn("you are copying SceneCheatCode")
	}
	return *this;
}

/**
 * @brief init the menu
 *
 * @return true if the init succed
 * @return false if the init failed
 */
bool			SceneCheatCode::init() {
	glm::vec2 winSz = _gui->gameInfo.windowSize;
	glm::vec2 tmpPos;
	glm::vec2 tmpSize;

	try {
		tmpPos.x = 0;
		tmpSize.x = winSz.x;
		tmpSize.y = 30;
		tmpPos.y = tmpSize.y;
		commandLine = &addTextInput(tmpPos, tmpSize, "/help to get help");
		commandLine->setTextFont("cheatcode")
			.setColor(glm::vec4(0.1, 0.1, 0.1, 1));
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
 * @return false if we need to quit the command line
 */
bool	SceneCheatCode::update() {
	ASceneMenu::update();

	if (Inputs::getKeyByScancode(SDL_SCANCODE_RETURN)) {
		return false;  // exit command line
	}

	return true;
}

/**
 * @brief called when the scene is loaded
 */
void SceneCheatCode::load() {
	ASceneMenu::load();
}

/**
 * @brief called when the scene is unloaded
 */
void SceneCheatCode::unload() {
	ASceneMenu::unload();
	commandLine->setText("");
}
