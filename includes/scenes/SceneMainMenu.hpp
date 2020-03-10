#pragma once

#include "ASceneMenu.hpp"

class SceneMainMenu : public ASceneMenu {
	public:
		// Constructors
		explicit SceneMainMenu(Gui * gui, float const &dtTime);
		virtual ~SceneMainMenu();
		SceneMainMenu(SceneMainMenu const &src);

		// Operators
		SceneMainMenu &operator=(SceneMainMenu const &rhs);
		friend std::ostream& operator<<(std::ostream& os, const SceneMainMenu& myClass);

		// AScene methods
		virtual bool		init();

	private:
		SceneMainMenu();
};
