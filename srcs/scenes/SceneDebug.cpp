#include <iostream>
#include <string>
#include "SceneDebug.hpp"
#include "SceneGame.hpp"

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
	float lineSz;

	try {
		uint32_t	fontH = ABaseUI::strHeight(DEBUG_FONT, DEBUG_FONT_SCALE) * 1.6;
		tmpPos.x = 3;
		tmpPos.y = winSz.y - 4 - fontH;
		tmpSize.x = winSz.x - (tmpPos.x * 2);
		tmpSize.y = fontH;
		lineSz = tmpSize.y * 1.2;

		_fpsText = &addText(tmpPos, tmpSize, "");
		_fpsText->setTextFont(DEBUG_FONT)
			.setTextScale(DEBUG_FONT_SCALE)
			.setTextColor(DEBUG_TEXT_COLOR)
			.setText(std::to_string(_fps) + "fps")
			.setTextAlign(TextAlign::LEFT)
			.setZ(1);

		tmpPos.y -= lineSz;
		_nbentitiesText = &addText(tmpPos, tmpSize, "[nb entities]");
		_nbentitiesText->setTextFont(DEBUG_FONT)
			.setTextScale(DEBUG_FONT_SCALE)
			.setTextColor(DEBUG_TEXT_COLOR)
			.setTextAlign(TextAlign::LEFT)
			.setZ(1);

		tmpPos.y -= lineSz;
		_nbenemiesText = &addText(tmpPos, tmpSize, "[nb enemies]");
		_nbenemiesText->setTextFont(DEBUG_FONT)
			.setTextScale(DEBUG_FONT_SCALE)
			.setTextColor(DEBUG_TEXT_COLOR)
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

	SceneGame & scGame = *reinterpret_cast<SceneGame *>(SceneManager::getScene(SceneNames::GAME));

	// toggle menu visibility
	if (Inputs::getKeyByScancodeDown(SDL_SCANCODE_F3))
		setVisible(!_visible);

	// update debug data
	if (_visible && getMs().count() - _lastUpdateMs.count() > UPDATE_DEBUG_DATA_MS) {
		_lastUpdateMs = getMs();
		_fps = SceneManager::getFps();
		_fpsText->setText(std::to_string(_fps) + "fps");
		_nbentitiesText->setText(std::to_string(scGame.entitiesCount.total) + " entities");
		_nbenemiesText->setText(std::to_string(scGame.entitiesCount.enemy) + " enemies");
	}
	// enable only in game
	bool visible = SceneManager::getSceneName() == SceneNames::GAME
		&& _visible
		&& scGame.state == GameState::PLAY
		&& scGame.entitiesCount.total > 0;
	_nbentitiesText->setEnabled(visible);
	_nbenemiesText->setEnabled(visible);

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
