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
		struct ButtonsStates {
			bool	exit;
			bool	cancel;
			bool	save;
		};
		ButtonsStates	_states;
		struct AllUI {
			ABaseUI	*exit;
			ABaseUI	*cancel;
			ABaseUI	*save;
			ABaseUI	*border;
		};
		AllUI			allUI;
		std::string		_lastSceneName;

	private:
		SceneExit();
		void			_updateUI();
};
