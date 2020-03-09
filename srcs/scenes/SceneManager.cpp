#include <unistd.h>
#include <chrono>

#include "SceneManager.hpp"
#include "bomberman.hpp"
#include "SceneGame.hpp"
#include "SceneMenu.hpp"

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
	_sceneMap.insert(std::pair<std::string, AScene *>(SceneNames::MAIN_MENU, new SceneMenu(_gui, _dtTime)));
	_sceneMap.insert(std::pair<std::string, AScene *>(SceneNames::GAME, new SceneGame(_gui, _dtTime)));

	for (auto it = _sceneMap.begin(); it != _sceneMap.end(); it++) {
		try {
			if (it->second->init() == false) {
				logErr("failed to init scene: " << it->first);
				return false;
			}
		} catch (std::exception &e) {
			logErr("Error : " << e.what());
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
		logErr("invalid scnene name: " << _scene << " in loadScene");
	}
	_sceneMap[_scene]->unload();  // unload last scene
	_scene = name;
	_sceneMap[_scene]->load();  // load new scene
	return _sceneMap[_scene];
}

/* execption */
SceneManager::SceneManagerException::SceneManagerException()
: std::runtime_error("SceneManager Exception") {}

SceneManager::SceneManagerException::SceneManagerException(const char* whatArg)
: std::runtime_error(std::string(std::string("SceneManagerException: ") + whatArg).c_str()) {}
