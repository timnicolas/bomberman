#pragma once

#include "ASceneMenu.hpp"
#include "ABaseUI.hpp"

/**
 * @brief Menu to choose dificulty (when starting a new game)
 */
class SceneDifficulty : public ASceneMenu {
	public:
		// Constructors
		SceneDifficulty(Gui * gui, float const &dtTime);
		virtual ~SceneDifficulty();
		SceneDifficulty(SceneDifficulty const &src);

		// Operators
		SceneDifficulty &operator=(SceneDifficulty const &rhs);
		friend std::ostream& operator<<(std::ostream& os, const SceneDifficulty& myClass);

		// Methods
		virtual bool		init();
		virtual void		load();
		virtual bool		update();

	protected:
		/**
		 * @brief All buttons states
		 */
		struct ButtonsStates {
			bool	beginner;  /**< True if we clicked on the button beginner */
			bool	easy;  /**< True if we clicked on the button easy */
			bool	medium;  /**< True if we clicked on the button medium */
			bool	hardCore;  /**< True if we clicked on the button hardCore */
			bool	menu;  /**< True if we clicked on the menu button */
		};
		ButtonsStates	_states;  /**< All buttons states */
		/**
		 * @brief All UI elements
		 */
		struct AllUI {
			ABaseUI	*beginner;  /**< UI for beginner element */
			ABaseUI	*easy;  /**< UI for easy element */
			ABaseUI	*medium;  /**< UI for medium element */
			ABaseUI	*hardCore;  /**< UI for hardCore element */
			ABaseUI	*border;  /**< UI for border element */
		};
		AllUI		allUI;  /**< All UI elements */


	private:
		SceneDifficulty();
		void		_updateUI();
};
