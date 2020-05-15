#pragma once

#include "ASceneMenu.hpp"

/**
 * @brief menu for game over
 */
class SceneGameOver : public ASceneMenu {
	public:
		// Constructors
		SceneGameOver(Gui * gui, float const &dtTime);
		virtual ~SceneGameOver();
		SceneGameOver(SceneGameOver const &src);

		// Operators
		SceneGameOver &operator=(SceneGameOver const &rhs);
		friend std::ostream& operator<<(std::ostream& os, const SceneGameOver& myClass);

		// Methods
		virtual bool		init();
		virtual bool		update();
		virtual void		load();
		virtual bool		draw();

	protected:
		/**
		 * @brief All buttons states
		 */
		struct ButtonsStates {
			bool	restart;  /**< True if we click on the button restart */
			bool	menu;  /**< True if we click on the button menu */
			bool	exit;  /**< True if we click on the button exit */
		};
		ButtonsStates	_states;  /**< All buttons states */
		std::string		_lastSceneName;  /**< Last scene name if we want to go back */

	private:
		SceneGameOver();
};
