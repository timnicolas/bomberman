#pragma once

#include "ASceneMenu.hpp"
#include "ABaseUI.hpp"

/**
 * @brief End of the game (after all levels finished)
 */
class SceneEndGame : public ASceneMenu {
	public:
		// Constructors
		SceneEndGame(Gui * gui, float const &dtTime);
		virtual ~SceneEndGame();
		SceneEndGame(SceneEndGame const &src);

		// Operators
		SceneEndGame &operator=(SceneEndGame const &rhs);
		friend std::ostream& operator<<(std::ostream& os, const SceneEndGame& myClass);

		// Methods
		virtual bool		init();
		virtual void		load();
		virtual bool		update();
		virtual bool		draw();

	protected:
		/**
		 * @brief All buttons states
		 */
		struct ButtonsStates {
			bool	exit;  /**< True if we clicked on the exit button */
		};
		ButtonsStates	_states;  /**< All buttons states */
		/**
		 * @brief All UI elements
		 */
		struct AllUI {
		};
		AllUI		allUI;  /**< All UI elements */

	private:
		SceneEndGame();
		void		_updateUI();
};
