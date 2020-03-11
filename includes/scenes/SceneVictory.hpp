#pragma once

#include "ASceneMenu.hpp"

/**
 * @brief this is the victory menu
 */
class SceneVictory : public ASceneMenu {
	public:
		// Constructors
		SceneVictory(Gui * gui, float const &dtTime);
		virtual ~SceneVictory();
		SceneVictory(SceneVictory const &src);

		// Operators
		SceneVictory &operator=(SceneVictory const &rhs);
		friend std::ostream& operator<<(std::ostream& os, const SceneVictory& myClass);

		// Methods
		virtual bool		init();
		virtual bool		update();
		virtual void		load();

	protected:
		struct ButtonsStates {
			bool	nextLevel;
			bool	restart;
			bool	menu;
			bool	exit;
		};
		ButtonsStates	_states;
		std::string		_lastSceneName;

		bool			_initBG();

	private:
		SceneVictory();
};
