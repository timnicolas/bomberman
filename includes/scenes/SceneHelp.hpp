#pragma once

#include "ASceneMenu.hpp"
#include "ABaseUI.hpp"

/**
 * @brief main menu (opened at the game startup)
 */
class SceneHelp : public ASceneMenu {
	public:
		// Constructors
		SceneHelp(Gui * gui, float const &dtTime);
		virtual ~SceneHelp();
		SceneHelp(SceneHelp const &src);

		// Operators
		SceneHelp &operator=(SceneHelp const &rhs);
		friend std::ostream& operator<<(std::ostream& os, const SceneHelp& myClass);

		// Methods
		virtual bool		init();
		virtual void		load();
		virtual bool		update();

	protected:
		struct ButtonsStates {
			bool	menu;
		};
		ButtonsStates	_states;
		struct AllUI {
			ABaseUI	*detonatorText;
		};
		AllUI		allUI;

	private:
		SceneHelp();
};
