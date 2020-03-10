#pragma once

#include "ASceneMenu.hpp"

class SceneExit : public ASceneMenu {
	public:
		// Constructors
		explicit SceneExit(Gui * gui, float const &dtTime);
		virtual ~SceneExit();
		SceneExit(SceneExit const &src);

		// Operators
		SceneExit &operator=(SceneExit const &rhs);
		friend std::ostream& operator<<(std::ostream& os, const SceneExit& myClass);

		// Methods
		virtual bool		init();
		virtual bool		update();

	protected:
		struct ButtonsStates {
			bool	exit;
			bool	cancel;
		};
		ButtonsStates	_states;

		bool			_initBG();

	private:
		SceneExit();
};
