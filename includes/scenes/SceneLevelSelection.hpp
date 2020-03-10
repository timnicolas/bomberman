#pragma once

#include "ASceneMenu.hpp"

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
			bool		menu;
			bool		lastLevel;
			bool		nextLevel;
		};
		ButtonsStates	_states;

		uint32_t		_currentLvl;

		bool			_initBG();

	private:
		SceneLevelSelection();
};
