#pragma once

#include "ASceneMenu.hpp"

/**
 * @brief main menu (opened at the game startup)
 */
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
			bool	newGame;
			bool	loadGame;
			bool	loadSettings;
			bool	exit;
		};
		ButtonsStates	_states;

	private:
		SceneMainMenu();
};
