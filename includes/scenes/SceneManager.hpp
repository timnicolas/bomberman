#pragma once

#include <stdexcept>
#include <map>
#include "AScene.hpp"
#include "Inputs.hpp"
#include "Gui.hpp"

/*
	this object can manage all scenes (AScene):
		- load scene
		- update & draw scene
*/

namespace SceneNames {
	static std::string const MAIN_MENU = "mainMenu";
	static std::string const GAME = "game";
	static std::string const EXIT = "exit";
}

class SceneManager {
	public:
		SceneManager();
		virtual ~SceneManager();

		static SceneManager &		get();
		static bool					init();
		static bool					run();
		static AScene *				loadScene(std::string const & name);
		static std::string const &	getSceneName();
		static void					quit();

		/* Exceptions */
		class SceneManagerException : public std::runtime_error {
		public:
			SceneManagerException();
			explicit SceneManagerException(const char* whatArg);
		};

	protected:
		GameInfo	_gameInfo;
		Gui *		_gui;
		float		_dtTime;
		std::string	_scene;  // the name of the current scene
		std::map<std::string, AScene *>	_sceneMap;  // all scene (in a map)

		bool				_init();
		bool				_run();
		AScene *			_loadScene(std::string const & name);
		std::string const &	_getSceneName() const;
		void				_quit();

	private:
		SceneManager(SceneManager const & src);
		SceneManager & operator=(SceneManager const & rhs);
};
