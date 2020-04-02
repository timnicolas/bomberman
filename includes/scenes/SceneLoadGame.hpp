#pragma once

#include "ASceneMenu.hpp"

/**
 * @brief menu for game over
 */
class SceneLoadGame : public ASceneMenu {
	public:
		// Constructors
		SceneLoadGame(Gui * gui, float const &dtTime);
		virtual ~SceneLoadGame();
		SceneLoadGame(SceneLoadGame const &src);

		// Operators
		SceneLoadGame &operator=(SceneLoadGame const &rhs);
		friend std::ostream& operator<<(std::ostream& os, const SceneLoadGame& myClass);

		// Methods
		virtual bool		init();
		virtual void		load();
		virtual bool		update();

	protected:
		struct ButtonsStates {
			bool	restart;
			bool	menu;
			bool	exit;
		};
		ButtonsStates	_states;
		std::string		_lastSceneName;

	private:
		SceneLoadGame();
};
