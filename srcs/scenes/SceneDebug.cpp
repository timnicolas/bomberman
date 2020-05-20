#include "SceneDebug.hpp"

#include <iostream>
#include <string>

/**
 * @brief Construct a new Scene Debug:: Scene Debug object
 *
 * @param gui A pointer on the gui object
 * @param dtTime A reference to the delta time
 */
SceneDebug::SceneDebug(Gui * gui, float const &dtTime)
: ASceneMenu(gui, dtTime)
{
	_draw3dMenu = false;
	_lastUpdateMs = getMs();
	_fps = SceneManager::getFps();
}

/**
 * @brief Destroy the Scene Debug:: Scene Debug object
 */
SceneDebug::~SceneDebug() {
}

/**
 * @brief Construct a new Scene Debug:: Scene Debug object
 *
 * @param src The object to do the copy
 */
SceneDebug::SceneDebug(SceneDebug const &src)
: ASceneMenu(src) {
	*this = src;
}

/**
 * @brief Copy this object
 *
 * @param rhs The object to copy
 * @return SceneDebug& A reference to the copied object
 */
SceneDebug &SceneDebug::operator=(SceneDebug const &rhs) {
	if (this != &rhs) {
		logWarn("you are copying SceneDebug")
	}

	return *this;
}

/**
 * @brief init the menu
 *
 * @return true if the init succeed
 * @return false if the init failed
 */
bool SceneDebug::init() {
	/* create UI */
	glm::vec2 winSz = _gui->gameInfo.windowSize;
	glm::vec2 tmpPos;
	glm::vec2 tmpSize;

	try {
		uint32_t	fontH = ABaseUI::strHeight(DEBUG_FONT, DEBUG_FONT_SCALE) * 1.6;
		tmpPos.x = 3;
		tmpPos.y = winSz.y - 4 - fontH;
		tmpSize.x = winSz.x - (tmpPos.x * 2);
		tmpSize.y = fontH;

		_fpsText = &addText(tmpPos, tmpSize, "");
		_fpsText->setTextFont(DEBUG_FONT)
			.setTextScale(DEBUG_FONT_SCALE)
			.setTextColor(DEBUG_TEXT_COLOR)
			.setText(std::to_string(_fps) + "fps")
			.setTextAlign(TextAlign::LEFT)
			.setZ(1);

		setVisible(false);
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
bool SceneDebug::update() {
	ASceneMenu::update();

	// toggle menu visibility
	if (Inputs::getKeyByScancodeDown(SDL_SCANCODE_F3))
		setVisible(!_visible);

	// update debug data
	if (_visible && getMs().count() - _lastUpdateMs.count() > UPDATE_DEBUG_DATA_MS) {
		_lastUpdateMs = getMs();
		_fps = SceneManager::getFps();
		_fpsText->setText(std::to_string(_fps) + "fps");
	}

	return true;
}

// -- Setters ------------------------------------------------------------------
/**
 * @brief Set the menu visible or invisible
 *
 * @param visible If is visible
 */
void	SceneDebug::setVisible(bool visible) {
	_visible = visible;
	_fpsText->setEnabled(_visible);
}
