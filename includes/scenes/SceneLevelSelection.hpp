#pragma once

#include "ASceneMenu.hpp"

#define TRANSITION_SPEED 0.05

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
		int32_t			_currentLvl;

		float			_transition;  // used to do a smooth transition

		void			_setLevel(int32_t level);
		bool			_initBG();

	private:
		SceneLevelSelection();
};
