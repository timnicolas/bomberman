#pragma once

#include "ASceneMenu.hpp"
#include "ABaseUI.hpp"

/**
 * @brief main menu (opened at the game startup)
 */
class SceneDifficulty : public ASceneMenu {
	public:
		// Constructors
		SceneDifficulty(Gui * gui, float const &dtTime);
		virtual ~SceneDifficulty();
		SceneDifficulty(SceneDifficulty const &src);

		// Operators
		SceneDifficulty &operator=(SceneDifficulty const &rhs);
		friend std::ostream& operator<<(std::ostream& os, const SceneDifficulty& myClass);

		// Methods
		virtual bool		init();
		virtual void		load();
		virtual bool		update();

	protected:
		struct ButtonsStates {
			bool	easy;
			bool	medium;
			bool	hardCore;
			bool	menu;
		};
		ButtonsStates	_states;
		struct AllUI {
			ABaseUI	*easy;
			ABaseUI	*medium;
			ABaseUI	*hardCore;
			ABaseUI	*menu;
			ABaseUI	*border;
		};
		AllUI		allUI;


	private:
		SceneDifficulty();
		void		_updateUI();
};
