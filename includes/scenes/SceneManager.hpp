#pragma once

#include <stdexcept>
#include "AScene.hpp"
#include "Inputs.hpp"
#include "Gui.hpp"

/*
	this object can manage all scenes (AScene):
		- load scene
		- update & draw scene
*/

class SceneManager {
	public:
		SceneManager();
		SceneManager(SceneManager const & src);
		virtual ~SceneManager();

		bool init();
		bool run();

		SceneManager & operator=(SceneManager const & rhs);

		/* Exceptions */
		class SceneManagerException : public std::runtime_error {
		public:
			SceneManagerException();
			explicit SceneManagerException(const char* what_arg);
		};

	protected:
		AScene *	_scene;
		GameInfo	_gameInfo;
		Gui *		_gui;
		float		_dtTime;
};
