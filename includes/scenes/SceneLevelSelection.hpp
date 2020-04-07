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

	protected:
		struct ButtonsStates {
			uint32_t	firstLevelID;
			uint32_t	nbLevel;
			bool		loadLevel;
			bool		menu;
			bool		lastLevel;
			bool		nextLevel;
		};
		ButtonsStates	_states;
		struct AllUI {
			ABaseUI		*text;
			ABaseUI		*score;
		};
		AllUI			allUI;
		int32_t			_currentLvl;

		float			_transition;  // used to do a smooth transition

		void			_setLevel(int32_t level);

	private:
		SceneLevelSelection();
};
