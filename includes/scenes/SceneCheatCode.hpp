#pragma once

#include "ASceneMenu.hpp"
#include "TextInputUI.hpp"

/**
 * @brief this is the cheat code command line
 */
class SceneCheatCode : public ASceneMenu {
	public:
		// Constructors
		SceneCheatCode(Gui * gui, float const &dtTime);
		virtual ~SceneCheatCode();
		SceneCheatCode(SceneCheatCode const &src);

		// Operators
		SceneCheatCode &operator=(SceneCheatCode const &rhs);
		friend std::ostream& operator<<(std::ostream& os, const SceneCheatCode& myClass);

		// Methods
		virtual bool		init();
		virtual bool		update();
		virtual void		load();
		virtual void		unload();

	private:
		SceneCheatCode();

		TextInputUI *	commandLine;
};
