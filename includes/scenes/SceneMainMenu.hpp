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

		// Methods
		virtual bool		init();
		virtual void		load();
		virtual bool		update();

	protected:
		/**
		 * @brief All buttons states
		 */
		struct ButtonsStates {
			bool	continueGame;  /**< True if we clicked on the button continueGame */
			bool	save;  /**< True if we clicked on the button save */
			bool	newGame;  /**< True if we clicked on the button newGame */
			bool	loadGame;  /**< True if we clicked on the button loadGame */
			bool	loadSettings;  /**< True if we clicked on the button loadSettings */
			bool	exit;  /**< True if we clicked on the button exit */
		};
		ButtonsStates	_states;  /**< All buttons states */
		/**
		 * @brief All UI elements
		 */
		struct AllUI {
			ABaseUI	*continueGame;  /**< UI for continueGame element */
			ABaseUI	*save;  /**< UI for save element */
			ABaseUI	*newGame;  /**< UI for newGame element */
			ABaseUI	*loadGame;  /**< UI for loadGame element */
			ABaseUI	*loadSettings;  /**< UI for loadSettings element */
			ABaseUI	*exit;  /**< UI for exit element */
			ABaseUI	*border;  /**< UI for border element */
		};
		AllUI		allUI;  /**< All UI elements */


	private:
		SceneMainMenu();
		void		_updateUI();
};
