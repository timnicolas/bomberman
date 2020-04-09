#pragma once

#include "ASceneMenu.hpp"
#include "ABaseUI.hpp"

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
		virtual void		load();
		virtual bool		update();

	protected:
		struct ButtonsStates {
			bool	continueGame;
			bool	save;
			bool	newGame;
			bool	loadGame;
			bool	loadSettings;
			bool	exit;
		};
		ButtonsStates	_states;
		struct AllUI {
			ABaseUI	*continueGame;
			ABaseUI	*save;
			ABaseUI	*newGame;
			ABaseUI	*loadGame;
			ABaseUI	*loadSettings;
			ABaseUI	*exit;
			ABaseUI	*border;
		};
		AllUI		allUI;


	private:
		SceneMainMenu();
		void		_updateUI();
};
