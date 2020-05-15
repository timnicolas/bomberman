#pragma once

#include "ASceneMenu.hpp"

#define TRANSITION_SPEED 0.05

/**
 * @brief menu for the level selection
 */
class SceneLevelSelection : public ASceneMenu {
	public:
		// Constructors
		explicit SceneLevelSelection(Gui * gui, float const &dtTime);
		virtual ~SceneLevelSelection();
		SceneLevelSelection(SceneLevelSelection const &src);

		// Operators
		SceneLevelSelection &operator=(SceneLevelSelection const &rhs);
		friend std::ostream& operator<<(std::ostream& os, const SceneLevelSelection& myClass);

		// Methods
		virtual bool		init();
		virtual bool		update();
		virtual void		load();
		void				setLevel(int32_t level, bool enableTransition = true);
		uint32_t			getCurLevel() const;
		uint32_t			getNbLevel() const;

	protected:
		/**
		 * @brief All buttons states
		 */
		struct ButtonsStates {
			uint32_t	firstLevelID;  /**< Id of UI for the first level */
			uint32_t	nbLevel;  /**< Number of levels */
			bool		loadLevel;  /**< True if we clicked on the button loadLevel */
			bool		menu;  /**< True if we clicked on the button menu */
			bool		lastLevel;  /**< True if we clicked on the button lastLevel */
			bool		nextLevel;  /**< True if we clicked on the button nextLevel */
		};
		ButtonsStates	_states;  /**< All buttons states */
		/**
		 * @brief All UI elements
		 */
		struct AllUI {
			ABaseUI *	text;  /**< UI for text element */
			ABaseUI *	score;  /**< UI for score element */
			ABaseUI *	leftArrow;  /**< UI for leftArrow element */
			ABaseUI *	rightArrow;  /**< UI for rightArrow element */
		};
		AllUI			allUI;  /**< All UI elements */
		int32_t			_currentLvl;  /**< Current level ID */

		float			_transition;  /**< Used to do a smooth transition */

	private:
		SceneLevelSelection();
};
