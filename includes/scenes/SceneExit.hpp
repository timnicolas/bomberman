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

	protected:
		struct ButtonsStates {
			bool	exit;
			bool	cancel;
		};
		ButtonsStates	_states;
		std::string		_lastSceneName;

		bool			_initBG();

	private:
		SceneExit();
};
