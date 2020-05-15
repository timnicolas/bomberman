#pragma once

#include "ASceneMenu.hpp"

/**
 * @brief menu for exit
 */
class SceneExit : public ASceneMenu {
	public:
		// Constructors
		SceneExit(Gui * gui, float const &dtTime);
		virtual ~SceneExit();
		SceneExit(SceneExit const &src);

		// Operators
		SceneExit &operator=(SceneExit const &rhs);
		friend std::ostream& operator<<(std::ostream& os, const SceneExit& myClass);

		// Methods
		virtual bool		init();
		virtual bool		update();
		virtual void		load();
		virtual void		unload();
		virtual bool		draw();

	protected:
		/**
		 * @brief All buttons states
		 */
		struct ButtonsStates {
			bool	exit;  /**< True if we clicked on exit button */
			bool	cancel;  /**< True if we clicked on cancel button */
			bool	save;  /**< True if we clicked on save button */
		};
		ButtonsStates	_states;  /**< All buttons states */
		/**
		 * @brief All UI elements
		 */
		struct AllUI {
			ABaseUI	*exit;  /**< UI for exit element */
			ABaseUI	*cancel;  /**< UI for cancel element */
			ABaseUI	*save;  /**< UI for save element */
			ABaseUI	*border;  /**< UI for border element */
		};
		AllUI			allUI;  /**< All UI elements */
		std::string		_lastSceneName;  /**< The last scene name (used if we cancel) */

	private:
		SceneExit();
		void			_updateUI();
};
