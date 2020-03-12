#include <unistd.h>
#include <chrono>

#include "SceneManager.hpp"
#include "bomberman.hpp"

/* import all scenes */
#include "SceneMainMenu.hpp"
#include "SceneLevelSelection.hpp"
#include "SceneGame.hpp"
#include "ScenePause.hpp"
#include "SceneGameOver.hpp"
#include "SceneVictory.hpp"
#include "SceneExit.hpp"
#include "SceneSettings.hpp"

SceneManager::SceneManager()
: _gameInfo(),
  _gui(nullptr),
  _dtTime(0.0f),
  _scene(SceneNames::MAIN_MENU)
{}

SceneManager::SceneManager(SceneManager const & src) {
	*this = src;
}

SceneManager::~SceneManager() {
	delete _gui;
	for (auto it = _sceneMap.begin(); it != _sceneMap.end(); it++) {
		delete it->second;
	}
	_sceneMap.clear();
}


SceneManager & SceneManager::operator=(SceneManager const & rhs) {
	(void)rhs;
	logErr("You sould never call copy opertor for SceneManager");
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
 * @return true if success
 * @return false if failure
 */
bool SceneManager::init() {
	return SceneManager::get()._init();
}
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

	// create and init all scene
	_sceneMap.insert(std::pair<std::string, AScene *>(SceneNames::MAIN_MENU, new SceneMainMenu(_gui, _dtTime)));
	_sceneMap.insert(std::pair<std::string, AScene *>(SceneNames::LEVEL_SELECTION,
		new SceneLevelSelection(_gui, _dtTime)));
	_sceneMap.insert(std::pair<std::string, AScene *>(SceneNames::GAME, new SceneGame(_gui, _dtTime)));
	_sceneMap.insert(std::pair<std::string, AScene *>(SceneNames::PAUSE, new ScenePause(_gui, _dtTime)));
	_sceneMap.insert(std::pair<std::string, AScene *>(SceneNames::GAME_OVER, new SceneGameOver(_gui, _dtTime)));
	_sceneMap.insert(std::pair<std::string, AScene *>(SceneNames::VICTORY, new SceneVictory(_gui, _dtTime)));
	_sceneMap.insert(std::pair<std::string, AScene *>(SceneNames::EXIT, new SceneExit(_gui, _dtTime)));
	_sceneMap.insert(std::pair<std::string, AScene *>(SceneNames::SETTINGS, new SceneSettings(_gui, _dtTime)));

	for (auto it = _sceneMap.begin(); it != _sceneMap.end(); it++) {
		try {
			if (it->second->init() == false) {
				logErr("failed to init scene: " << it->first);
				return false;
			}
		} catch (std::exception &e) {
			logErr("Error : " << e.what());
			return false;
		}
	}

	_sceneMap[_scene]->load();  // load first scene
	return true;
}

/**
 * @brief this is the main game loop to update & draw everythings
 *
 * @return true if success
 * @return false if failure
 */
bool SceneManager::run() {
	return SceneManager::get()._run();
}
bool SceneManager::_run() {
	float						fps = 1000 / s.j("screen").u("fps");
	std::chrono::milliseconds	lastLoopMs = getMs();

	while (true) {
		if (_sceneMap.find(_scene) == _sceneMap.end()) {
			logWarn("invalid scnene name: " << _scene);
		}
		else {
			// update dtTime
			_dtTime = (getMs().count() - lastLoopMs.count()) / 1000.0;
			lastLoopMs = getMs();

			// get inputs
			Inputs::update();
			_gui->updateInput(_dtTime);

			// update the scene
			if (_sceneMap[_scene]->update() == false) {
				return false;
			}

			// draw the gui
			_gui->preDraw();
			if (_sceneMap[_scene]->draw() == false) {
				return false;
			}
			_gui->postDraw();

			// quit if it's the end of the game
			if (_gameInfo.quit) {
				logInfo("exit game")
				break;
			}
		}

		// fps
		std::chrono::milliseconds loopDuration = getMs() - lastLoopMs;
		if (loopDuration.count() > fps) {
			#if DEBUG_FPS_LOW == true
				if (!firstLoop)
					logDebug("update loop slow -> " << loopDuration.count() << "ms / " << fps << "ms (" << FPS << "fps)");
			#endif
		}
		else {
			usleep((fps - loopDuration.count()) * 1000);
		}
		#if DEBUG_FPS_LOW == true
			firstLoop = false;
		#endif
	}
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
AScene * SceneManager::_loadScene(std::string const & name) {
	if (get()._sceneMap.find(name) == get()._sceneMap.end()) {
		logErr("invalid scnene name: " << name << " in loadScene");
		return _sceneMap[_scene];
	}
	_sceneMap[_scene]->unload();  // unload last scene
	_sceneMap[name]->load();  // load new scene (getScene return the name of the last scene)
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
AScene * SceneManager::_getScene(std::string const & name) {
	if (get()._sceneMap.find(name) == get()._sceneMap.end()) {
		logErr("invalid scnene name: " << name << " in getScene");
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
std::string const & SceneManager::_getSceneName() const {
	return _scene;
}

/**
 * @brief quit the game
 */
void SceneManager::quit() {
	SceneManager::get()._quit();
}
void SceneManager::_quit() {
	_gameInfo.quit = true;
}

/* execption */
SceneManager::SceneManagerException::SceneManagerException()
: std::runtime_error("SceneManager Exception") {}

SceneManager::SceneManagerException::SceneManagerException(const char* whatArg)
: std::runtime_error(std::string(std::string("SceneManagerException: ") + whatArg).c_str()) {}
