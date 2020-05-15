#pragma once

#include "ASceneMenu.hpp"

/**
 * @brief this is the victory menu
 */
class SceneVictory : public ASceneMenu {
	public:
		// Constructors
		SceneVictory(Gui * gui, float const &dtTime);
		virtual ~SceneVictory();
		SceneVictory(SceneVictory const &src);

		// Operators
		SceneVictory &operator=(SceneVictory const &rhs);
		friend std::ostream& operator<<(std::ostream& os, const SceneVictory& myClass);

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
			bool	nextLevel;  /**< True if we clicked on the button nextLevel */
			bool	restart;  /**< True if we clicked on the button restart */
			bool	menu;  /**< True if we clicked on the button menu */
			bool	exit;  /**< True if we clicked on the button exit */
		};
		ButtonsStates	_states;  /**< All buttons states */
		std::string		_lastSceneName;  /**< The last loaded scene name */
		/**
		 * @brief Stats about game to draw on the screen
		 */
		struct StatUI {
			ABaseUI *				level;  /**< Current level */
			std::vector<ABaseUI *>	stats;  /**< All stats elements */
		};
		StatUI			statistics;  /**< Master stats */

	private:
		SceneVictory();
};
