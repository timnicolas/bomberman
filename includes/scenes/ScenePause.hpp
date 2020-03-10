#pragma once

#include "ASceneMenu.hpp"

class ScenePause : public ASceneMenu {
	public:
		// Constructors
		explicit ScenePause(Gui * gui, float const &dtTime);
		virtual ~ScenePause();
		ScenePause(ScenePause const &src);

		// Operators
		ScenePause &operator=(ScenePause const &rhs);
		friend std::ostream& operator<<(std::ostream& os, const ScenePause& myClass);

		// Methods
		virtual bool		init();
		virtual bool		update();
		virtual void		load();

	protected:
		struct ButtonsStates {
			bool	resume;
			bool	menu;
			bool	exit;
		};
		ButtonsStates	_states;
		std::string		_lastSceneName;

		bool			_initBG();

	private:
		ScenePause();
};
