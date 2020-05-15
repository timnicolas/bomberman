#pragma once

#include "ASceneMenu.hpp"

/**
 * @brief this is the pause menu
 */
class ScenePause : public ASceneMenu {
	public:
		// Constructors
		ScenePause(Gui * gui, float const &dtTime);
		virtual ~ScenePause();
		ScenePause(ScenePause const &src);

		// Operators
		ScenePause &operator=(ScenePause const &rhs);
		friend std::ostream& operator<<(std::ostream& os, const ScenePause& myClass);

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
			bool	resume;  /**< True if we clicked on the button resume */
			bool	restart;  /**< True if we clicked on the button restart */
			bool	menu;  /**< True if we clicked on the button menu */
			bool	exit;  /**< True if we clicked on the button exit */
		};
		ButtonsStates	_states;  /**< All buttons states */
		std::string		_lastSceneName;  /**< The last scene loaded (if we resume) */

	private:
		ScenePause();
};
