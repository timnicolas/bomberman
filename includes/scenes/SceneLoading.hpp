#pragma once

#include "ASceneMenu.hpp"
#include "ABaseUI.hpp"

/**
 * @brief loading game scene
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
