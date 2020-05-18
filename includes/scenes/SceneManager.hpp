#pragma once

#include <stdexcept>
#include <map>
#include "AScene.hpp"
#include "Inputs.hpp"
#include "Gui.hpp"

/**
 * @brief this is the list of all existing scenes
 */
namespace SceneNames {
	static std::string const MAIN_MENU = "mainMenu";
	static std::string const LEVEL_SELECTION = "levelSelection";
	static std::string const DIFFICULTY = "difficulty";
	static std::string const GAME = "game";
	static std::string const PAUSE = "pause";
	static std::string const GAME_OVER = "gameOver";
	static std::string const VICTORY = "victory";
	static std::string const EXIT = "exit";
	static std::string const SETTINGS = "settings";
	static std::string const LOADGAME = "loadGame";
	static std::string const CHEAT_CODE = "cheatCode";
	static std::string const END_GAME = "endGame";
	static std::string const DEBUG_MENU = "debugMenu";
	static std::string const LOADING = "loading";
}  // namespace SceneNames

/**
 * @brief this object can manage all scenes (AScene objects)
 *
 * you can load a scene (SceneManager::loadScene) and his update & draw will be automatically updated
 */
class SceneManager {
	public:
		SceneManager();
		virtual ~SceneManager();

		static SceneManager &		get();
		static bool					init();
		static bool					run();
		static AScene *				loadScene(std::string const & name);
		static AScene *				getScene(std::string const & name);
		static std::string const &	getSceneName();
		static uint16_t				getFps();
		static bool					isSceneChangedInCurFrame();
		static void					openCheatCode(bool open);
		static void					openCheatCodeForTime(uint64_t ms);
		static bool					isCheatCodeOpen();
		static void					quit();

		/* Exceptions */
		/**
		 * @brief SceneManager exception
		 */
		class SceneManagerException : public std::runtime_error {
		public:
			SceneManagerException();
			/**
			 * @brief Construct a new Scene Manager Exception object
			 *
			 * @param whatArg Error message
			 */
			explicit SceneManagerException(const char* whatArg);
		};

	protected:
		bool		_isInit;  /**< True if SceneManager was init */
		GameInfo	_gameInfo;  /**< Game global informations */
		Gui *		_gui;  /**< Gui object */
		float		_dtTime;  /**< Delta time btw 2 loop calls */
		std::string	_scene;  /**< The name of the current scene */
		std::map<std::string, AScene *>	_sceneMap;  /**< All scenes (in a map) */
		bool		_isInCheatCode;  /**< True if we are in cheatcode */
		int64_t		_showCheatCodeTextTime;  /**< If > 0 -> number of ms to show the text information of cheatcode */
		uint16_t	_fps;  /**< Max FPS of the game */

		bool		_sceneLoadedCurrentFrame;  /**< True if a scene was loaded in the current frame */

		bool				_init();
		bool				_run(float maxFrameDuration);
		bool				_update();
		bool				_draw();
		AScene *			_loadScene(std::string const & name);
		AScene *			_getScene(std::string const & name);
		std::string const &	_getSceneName() const;
		bool				_isSceneChangedInCurFrame() const;
		void				_openCheatCode(bool open);
		void				_openCheatCodeForTime(uint64_t ms);
		bool				_isCheatCodeOpen() const;
		void				_quit();

	private:
		SceneManager(SceneManager const & src);
		SceneManager & operator=(SceneManager const & rhs);
};
