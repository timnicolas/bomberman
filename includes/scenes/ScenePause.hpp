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
		struct ButtonsStates {
			bool	resume;
			bool	restart;
			bool	menu;
			bool	exit;
		};
		ButtonsStates	_states;
		std::string		_lastSceneName;

	private:
		ScenePause();
};
