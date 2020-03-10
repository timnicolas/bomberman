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
			bool	play;
			bool	menu;
		};
		ButtonsStates	_states;

		bool			_initBG();

	private:
		SceneLevelSelection();
};
