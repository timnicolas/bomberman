#include <unistd.h>
#include <chrono>

#include "SceneManager.hpp"
#include "bomberman.hpp"
#include "ABaseUI.hpp"
#include "ModelsManager.hpp"

/* import all scenes */
#include "SceneMainMenu.hpp"
#include "SceneLevelSelection.hpp"
#include "SceneDifficulty.hpp"
#include "SceneGame.hpp"
#include "ScenePause.hpp"
#include "SceneGameOver.hpp"
#include "SceneVictory.hpp"
#include "SceneExit.hpp"
#include "SceneSettings.hpp"
#include "SceneLoadGame.hpp"
#include "SceneCheatCode.hpp"
#include "SceneEndGame.hpp"
#include "SceneDebug.hpp"
#include "SceneLoading.hpp"
#include "SceneHelp.hpp"

/**
 * @brief Construct a new Scene Manager:: Scene Manager object
 */
SceneManager::SceneManager()
: _isInit(false),
  _gameInfo(),
  _gui(nullptr),
  _dtTime(0.0f),
  _scene(SceneNames::LOADING),
  _isInCheatCode(false),
  _showCheatCodeTextTime(0),
  _fps(60),
  _sceneLoadedCurrentFrame(false)
{}

/**
 * @brief Construct a new Scene Manager:: Scene Manager object
 *
 * @param src The object to do the copy
 */
SceneManager::SceneManager(SceneManager const & src) {
	*this = src;
}

/**
 * @brief Destroy the Scene Manager:: Scene Manager object
 */
SceneManager::~SceneManager() {
	if (!_isInit)
		return;
	SceneSettings & scSettings = *reinterpret_cast<SceneSettings*>(getScene(SceneNames::SETTINGS));
	if (scSettings.startFitToScreen && !s.j("graphics").b("fitToScreen")) {
		_gui->gameInfo.savedWindowSize.x = scSettings.getCurResolution().x;
		_gui->gameInfo.savedWindowSize.y = scSettings.getCurResolution().y;
	}
	s.j("graphics").i("width") = _gui->gameInfo.savedWindowSize.x;
	s.j("graphics").i("height") = _gui->gameInfo.savedWindowSize.y;
	s.j("graphics").b("fullscreen") = _gui->gameInfo.isSavedFullscreen;
	saveSettings(homeDir+SETTINGS_FILE);
	delete _gui;
	for (auto it = _sceneMap.begin(); it != _sceneMap.end(); it++) {
		delete it->second;
	}
	_sceneMap.clear();
}

/**
 * @brief Copy this object
 *
 * @param rhs The object to copy
 * @return SceneManager& A reference to the copied object
 */
SceneManager & SceneManager::operator=(SceneManager const & rhs) {
	(void)rhs;
	logErr("You should never call copy operator for SceneManager");
	return *this;
}

/**
 * @brief get the SceneManager
 *
 * @return SceneManager& the instance of the SceneManager
 */
SceneManager & SceneManager::get() {
	static SceneManager	instance;
	return instance;
}

/**
 * @brief init the SceneManager (load the first scene& load gui)
 *
 * @return false if failure
 */
bool SceneManager::init() {
	return SceneManager::get()._init();
}
/**
 * @brief init the SceneManager (load the first scene& load gui)
 *
 * @return false if failure
 */
bool SceneManager::_init() {
	if (_sceneMap.find(SceneNames::MAIN_MENU) != _sceneMap.end()) {
		logWarn("SceneManager::init already called");
		return false;
	}

	// create and init first gui
	_gui = new Gui(_gameInfo);
	if (!_gui->init()) {
		return false;
	}

	/* create the init scene */
	_scene = SceneNames::LOADING;
	_sceneMap.insert(std::pair<std::string, AScene *>(SceneNames::LOADING, new SceneLoading(_gui, _dtTime)));
	try {
		if (_sceneMap[_scene]->init() == false) {
			logErr("failed to init scene: loading");
			return false;
		}
	} catch (std::exception const & e) {
		logErr("Error : " << e.what());
		return false;
	}

	try {
		_sceneMap[_scene]->load();  // load first scene
	} catch (std::exception const & e) {
		logErr("Error : " << e.what());
		return false;
	}

	/* draw */
	Inputs::update();
	_gui->preDraw();
	// draw debug menu scene
	if (_sceneMap[_scene]->draw() == false) {
		return false;
	}
	_gui->postDraw();

	// load and init 3d models
	if (!ModelsManager::init(*(_gui->cam))) {
		return false;
	}

	// create and init all scene
	_sceneMap.insert(std::pair<std::string, AScene *>(SceneNames::MAIN_MENU, new SceneMainMenu(_gui, _dtTime)));
	_sceneMap.insert(std::pair<std::string, AScene *>(SceneNames::LEVEL_SELECTION,
		new SceneLevelSelection(_gui, _dtTime)));
	_sceneMap.insert(std::pair<std::string, AScene *>(SceneNames::DIFFICULTY,
		new SceneDifficulty(_gui, _dtTime)));
	_sceneMap.insert(std::pair<std::string, AScene *>(SceneNames::GAME, new SceneGame(_gui, _dtTime)));
	_sceneMap.insert(std::pair<std::string, AScene *>(SceneNames::PAUSE, new ScenePause(_gui, _dtTime)));
	_sceneMap.insert(std::pair<std::string, AScene *>(SceneNames::GAME_OVER, new SceneGameOver(_gui, _dtTime)));
	_sceneMap.insert(std::pair<std::string, AScene *>(SceneNames::VICTORY, new SceneVictory(_gui, _dtTime)));
	_sceneMap.insert(std::pair<std::string, AScene *>(SceneNames::EXIT, new SceneExit(_gui, _dtTime)));
	_sceneMap.insert(std::pair<std::string, AScene *>(SceneNames::SETTINGS, new SceneSettings(_gui, _dtTime)));
	_sceneMap.insert(std::pair<std::string, AScene *>(SceneNames::LOADGAME, new SceneLoadGame(_gui, _dtTime)));
	_sceneMap.insert(std::pair<std::string, AScene *>(SceneNames::CHEAT_CODE, new SceneCheatCode(_gui, _dtTime)));
	_sceneMap.insert(std::pair<std::string, AScene *>(SceneNames::END_GAME, new SceneEndGame(_gui, _dtTime)));
	_sceneMap.insert(std::pair<std::string, AScene *>(SceneNames::DEBUG_MENU, new SceneDebug(_gui, _dtTime)));
	_sceneMap.insert(std::pair<std::string, AScene *>(SceneNames::HELP, new SceneHelp(_gui, _dtTime)));

	for (auto it = _sceneMap.begin(); it != _sceneMap.end(); it++) {
		try {
			if (it->first == SceneNames::LOADING)
				continue;
			if (it->second->init() == false) {
				logErr("failed to init scene: " << it->first);
				return false;
			}
		} catch (std::exception const & e) {
			logErr("Error : " << e.what());
			return false;
		}
	}
	_isInit = true;
	return true;
}

/**
 * @brief this is the main game loop to update & draw everythings
 *
 * @return false if failure
 */
bool SceneManager::run() {
	float		maxFrameDuration = 1000 / s.j("screen").u("fps");

	return SceneManager::get()._run(maxFrameDuration);
}
/**
 * @brief this is the main game loop to update & draw everythings
 *
 * @return false if failure
 */
bool SceneManager::_run(float maxFrameDuration) {
	std::chrono::milliseconds	lastLoopMs = getMs();
	bool firstLoop = true;
	(void)firstLoop;

	while (true) {
		/* reset variables */
		_sceneLoadedCurrentFrame = false;
		_dtTime = (getMs().count() - lastLoopMs.count()) / 1000.0;
		lastLoopMs = getMs();
		_fps = 1 / _dtTime;

		if (_sceneMap.find(_scene) == _sceneMap.end()) {
			logWarn("invalid scene name: " << _scene);
		}
		else {
			/* update & draw scene */
			if (_update() == false)
				return false;
			if (_draw() == false)
				return false;
		}

		/* quit if it's the end of the game */
		if (_gameInfo.quit) {
			logDebug("exit game")
			break;
		}

		/* fps control */
		std::chrono::milliseconds loopDuration = getMs() - lastLoopMs;
		float	frameDuration = loopDuration.count();

		if (frameDuration > maxFrameDuration) {
			#if DEBUG_FPS_LOW == true
				if (!firstLoop) {
					logDebug("update loop slow -> " << frameDuration << "ms / " <<
					maxFrameDuration << "ms (" << s.j("screen").u("fps") << "fps)");
				}
			#endif
		}
		else {
			usleep((maxFrameDuration - frameDuration) * 1000);
		}
		firstLoop = false;
	}
	return true;
}

/**
 * @brief Update the current scene
 *
 * @return false if error
 */
bool SceneManager::_update() {
	/* get inputs */
	Inputs::update();

	/* update */
	ABaseUI::staticUpdate();
	_gui->preUpdate(_dtTime);
	bool cheatCodeClosed = false;

	/* debug menu */
	if (_sceneMap[SceneNames::DEBUG_MENU]->update() == false) {
		return false;
	}

	/* cheatcode */
	if (_isInCheatCode || _showCheatCodeTextTime > 0) {
		// text only mode
		if (_showCheatCodeTextTime > 0) {
			reinterpret_cast<SceneCheatCode *>(_sceneMap[SceneNames::CHEAT_CODE])->isCmdLnEnabled = false;
			_showCheatCodeTextTime -= _dtTime * 1000;
			if (_showCheatCodeTextTime < 0)
				_showCheatCodeTextTime = 0;
		}
		else {
			reinterpret_cast<SceneCheatCode *>(_sceneMap[SceneNames::CHEAT_CODE])->isCmdLnEnabled = true;
		}
		// update scene
		if (_sceneMap[SceneNames::CHEAT_CODE]->update() == false) {
			openCheatCode(false);  // close cheat code
			cheatCodeClosed = true;
		}
	}

	/* scene */
	if (!_isInCheatCode && !cheatCodeClosed) {
		// update the scene
		if (_sceneMap[_scene]->update() == false) {
			logErr("Unexpected error when updating scene");
			return false;
		}
	}

	if (isSceneChangedInCurFrame())
		_gui->disableExitForThisFrame();

	_gui->postUpdate(_dtTime);

	return true;
}

/**
 * @brief Draw the current scene
 *
 * @return false if error
 */
bool SceneManager::_draw() {
	/* draw */
	_gui->preDraw();

	// draw the scene
	if (_sceneMap[_scene]->draw() == false) {
		logErr("Unexpected error when drawing scene");
		return false;
	}

	// draw cheatcode scene
	if (_isInCheatCode || _showCheatCodeTextTime > 0) {
		if (_sceneMap[SceneNames::CHEAT_CODE]->draw() == false) {
			logErr("Unexpected error when drawing scene");
			return false;
		}
	}

	// draw debug menu scene
	if (_sceneMap[SceneNames::DEBUG_MENU]->draw() == false) {
		return false;
	}

	_gui->postDraw();

	return true;
}

/**
 * @brief load a scene from his name
 *
 * @param name the scene name
 * @return AScene* a pointer to the scene loaded
 */
AScene * SceneManager::loadScene(std::string const & name) {
	return SceneManager::get()._loadScene(name);
}
/**
 * @brief load a scene from his name
 *
 * @param name the scene name
 * @return AScene* a pointer to the scene loaded
 */
AScene * SceneManager::_loadScene(std::string const & name) {
	if (get()._sceneMap.find(name) == get()._sceneMap.end()) {
		logErr("invalid scene name: " << name << " in loadScene");
		return _sceneMap[_scene];
	}
	if (_scene == name) {
		return _sceneMap[_scene];
	}
	// scene changed, load the new
	_sceneLoadedCurrentFrame = true;
	_sceneMap[_scene]->unload();  // unload last scene
	try {
		_sceneMap[name]->load();  // load new scene (getScene return the name of the last scene)
	} catch (std::exception const &e) {
		logErr("Error: " << e.what());
	}
	_scene = name;
	return _sceneMap[_scene];
}

/**
 * @brief get a scene
 *
 * @param name the name of the scene to get
 * @return AScene* a pointer to the scene
 */
AScene * SceneManager::getScene(std::string const & name) {
	return SceneManager::get()._getScene(name);
}
/**
 * @brief get a scene
 *
 * @param name the name of the scene to get
 * @return AScene* a pointer to the scene
 */
AScene * SceneManager::_getScene(std::string const & name) {
	if (get()._sceneMap.find(name) == get()._sceneMap.end()) {
		logErr("invalid scene name: " << name << " in getScene");
		return _sceneMap[_scene];
	}
	return _sceneMap[name];
}

/**
 * @brief get the current scene name
 *
 * @return std::string const& the current scene name
 */
std::string const & SceneManager::getSceneName() {
	return SceneManager::get()._getSceneName();
}
/**
 * @brief get the current scene name
 *
 * @return std::string const& the current scene name
 */
std::string const & SceneManager::_getSceneName() const {
	return _scene;
}

/**
 * @brief get the current fps count
 *
 * @return uint16_t the current fps count
 */
uint16_t	SceneManager::getFps() {
	uint16_t clampVal = s.j("screen").u("fps");
	uint16_t clampFps = SceneManager::get()._fps;
	clampFps = (clampFps < clampVal) ? clampFps : clampVal;
	return clampFps;
}

/**
 * @brief Return if the scene has changed in the current frame
 *
 * @return true If the scene changed in the current frame
 */
bool SceneManager::isSceneChangedInCurFrame() {
	return SceneManager::get()._isSceneChangedInCurFrame();
}
/**
 * @brief Return if the scene has changed in the current frame
 *
 * @return true If the scene changed in the current frame
 */
bool SceneManager::_isSceneChangedInCurFrame() const {
	return _sceneLoadedCurrentFrame;
}

/**
 * @brief Open or force close cheat code command line
 *
 * @param open True to open cheat code command line
 */
void SceneManager::openCheatCode(bool open) {
	SceneManager::get()._openCheatCode(open);
}
/**
 * @brief Open or force close cheat code command line
 *
 * @param open True to open cheat code command line
 */
void SceneManager::_openCheatCode(bool open) {
	if (_isInCheatCode == open)  // if state didn't changed
		return;
	if (open) {
		_showCheatCodeTextTime = 0;
		if (reinterpret_cast<SceneCheatCode *>(_sceneMap[SceneNames::CHEAT_CODE])->getText().size() == 0)
			reinterpret_cast<SceneCheatCode *>(_sceneMap[SceneNames::CHEAT_CODE])->setText(CHEATCODE_DEF_TXT);
		_sceneMap[SceneNames::CHEAT_CODE]->load();
	}
	else {
		_sceneMap[SceneNames::CHEAT_CODE]->unload();
	}
	_isInCheatCode = open;
}

/**
 * @brief Open cheatcode text for a certain time
 *
 * @param ms The time to open cheatcode text
 */
void SceneManager::openCheatCodeForTime(uint64_t ms) {
	SceneManager::get()._openCheatCodeForTime(ms);
}
/**
 * @brief Open cheatcode text for a certain time
 *
 * @param ms The time to open cheatcode text
 */
void SceneManager::_openCheatCodeForTime(uint64_t ms) {
	if (ms == 0 || static_cast<int64_t>(ms) > _showCheatCodeTextTime) {
		_showCheatCodeTextTime = ms;
	}
}

/**
 * @brief Know if we are in cheat code mode
 *
 * @return true If cheat code command line is open
 */
bool SceneManager::isCheatCodeOpen() {
	return SceneManager::get()._isCheatCodeOpen();
}
/**
 * @brief Know if we are in cheat code mode
 *
 * @return true If cheat code command line is open
 */
bool SceneManager::_isCheatCodeOpen() const {
	return _isInCheatCode;
}

/**
 * @brief quit the game
 */
void SceneManager::quit() {
	SceneManager::get()._quit();
}
/**
 * @brief quit the game
 */
void SceneManager::_quit() {
	_gameInfo.quit = true;
}

/* exception */
/**
 * @brief Construct a new Scene Manager:: Scene Manager Exception:: Scene Manager Exception object
 */
SceneManager::SceneManagerException::SceneManagerException()
: std::runtime_error("SceneManager Exception") {}

/**
 * @brief Construct a new Scene Manager:: Scene Manager Exception:: Scene Manager Exception object
 *
 * @param whatArg Error message
 */
SceneManager::SceneManagerException::SceneManagerException(const char* whatArg)
: std::runtime_error(std::string(std::string("SceneManagerException: ") + whatArg).c_str()) {}
