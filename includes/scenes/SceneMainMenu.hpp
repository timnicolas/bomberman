#pragma once

#include "ASceneMenu.hpp"

class SceneMainMenu : public ASceneMenu {
	public:
		// Constructors
		SceneMainMenu(Gui * gui, float const &dtTime);
		virtual ~SceneMainMenu();
		SceneMainMenu(SceneMainMenu const &src);

		// Operators
		SceneMainMenu &operator=(SceneMainMenu const &rhs);
		friend std::ostream& operator<<(std::ostream& os, const SceneMainMenu& myClass);

		// Methods
		virtual bool		init();
		virtual bool		update();

	protected:
		struct ButtonsStates {
			bool	play;
			bool	loadSettings;
			bool	exit;
		};
		ButtonsStates	_states;

		bool			_initBG();

	private:
		SceneMainMenu();
};
