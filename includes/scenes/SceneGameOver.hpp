#pragma once

#include "ASceneMenu.hpp"

/**
 * @brief menu for game over
 */
class SceneGameOver : public ASceneMenu {
	public:
		// Constructors
		SceneGameOver(Gui * gui, float const &dtTime);
		virtual ~SceneGameOver();
		SceneGameOver(SceneGameOver const &src);

		// Operators
		SceneGameOver &operator=(SceneGameOver const &rhs);
		friend std::ostream& operator<<(std::ostream& os, const SceneGameOver& myClass);

		// Methods
		virtual bool		init();
		virtual bool		update();
		virtual void		load();

	protected:
		struct ButtonsStates {
			bool	restart;
			bool	menu;
			bool	exit;
		};
		ButtonsStates	_states;
		std::string		_lastSceneName;

		bool			_initBG();

	private:
		SceneGameOver();
};
