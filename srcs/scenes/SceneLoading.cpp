#include "SceneLoading.hpp"
#include "AudioManager.hpp"
#include "Save.hpp"

SceneLoading::SceneLoading(Gui * gui, float const &dtTime)
: ASceneMenu(gui, dtTime)
{
	_draw3dMenu = false;
}

SceneLoading::SceneLoading(SceneLoading const & src)
: ASceneMenu(src)
{
	*this = src;
}

SceneLoading::~SceneLoading() {}

SceneLoading & SceneLoading::operator=(SceneLoading const & rhs) {
	if (this != &rhs) {
		logWarn("you are copying SceneLoading")
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
bool			SceneLoading::init() {
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
void SceneLoading::load() {
	ASceneMenu::load();
}

/**
 * @brief this is the update function (called every frames)
 *
 * @return true if the update is a success
 * @return false if there are an error in update
 */
bool	SceneLoading::update() {
	ASceneMenu::update();
	SceneManager::loadScene(SceneNames::MAIN_MENU);
	return true;
}

/**
 * @brief this is the draw function (called every frames)
 *
 * @return true if the draw is a success
 * @return false if there are an error in draw
 */
bool SceneLoading::draw() {
	bool ret = true;
	ret = ASceneMenu::draw();
	return ret & true;
}
