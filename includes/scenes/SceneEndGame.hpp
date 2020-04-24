#pragma once

#include "ASceneMenu.hpp"
#include "ABaseUI.hpp"

/**
 * @brief End of the game (after all levels finished)
 */
class SceneEndGame : public ASceneMenu {
	public:
		// Constructors
		SceneEndGame(Gui * gui, float const &dtTime);
		virtual ~SceneEndGame();
		SceneEndGame(SceneEndGame const &src);

		// Operators
		SceneEndGame &operator=(SceneEndGame const &rhs);
		friend std::ostream& operator<<(std::ostream& os, const SceneEndGame& myClass);

		// Methods
		virtual bool		init();
		virtual void		load();
		virtual bool		update();
		virtual bool		draw();

	protected:
		struct ButtonsStates {
			bool	mainMenu;
			bool	save;
			bool	exit;
		};
		ButtonsStates	_states;
		struct AllUI {
			ABaseUI	*mainMenu;
			ABaseUI	*save;
			ABaseUI	*exit;
			ABaseUI	*border;
		};
		AllUI		allUI;


	private:
		SceneEndGame();
		void		_updateUI();
};
