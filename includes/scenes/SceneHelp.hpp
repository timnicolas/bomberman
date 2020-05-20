#pragma once

#include "ASceneMenu.hpp"
#include "ABaseUI.hpp"

/**
 * @brief main menu (opened at the game startup)
 */
class SceneHelp : public ASceneMenu {
	public:
		// Constructors
		SceneHelp(Gui * gui, float const &dtTime);
		virtual ~SceneHelp();
		SceneHelp(SceneHelp const &src);

		// Operators
		SceneHelp &operator=(SceneHelp const &rhs);

		// Methods
		virtual bool		init();
		virtual void		load();
		virtual bool		update();

	protected:
		/**
		 * @brief All buttons states
		 */
		struct ButtonsStates {
			bool	menu;  /**< True if we clicked on the menu button */
		};
		ButtonsStates	_states;  /**< All buttons states */
		/**
		 * @brief All UI elements
		 */
		struct AllUI {
			ABaseUI	*detonatorText;  /**< UI for text of detonator */
		};
		AllUI		allUI;  /**< All UI elements */

	private:
		SceneHelp();
};
