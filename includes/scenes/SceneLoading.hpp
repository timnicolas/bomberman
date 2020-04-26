#pragma once

#include "ASceneMenu.hpp"
#include "ABaseUI.hpp"

/**
 * @brief main menu (opened at the game startup)
 */
class SceneLoading : public ASceneMenu {
	public:
		// Constructors
		SceneLoading(Gui * gui, float const &dtTime);
		virtual ~SceneLoading();
		SceneLoading(SceneLoading const &src);

		// Operators
		SceneLoading &operator=(SceneLoading const &rhs);
		friend std::ostream& operator<<(std::ostream& os, const SceneLoading& myClass);

		// Methods
		virtual bool		init();
		virtual void		load();
		virtual bool		update();
		virtual bool		draw();

	private:
		SceneLoading();
};
