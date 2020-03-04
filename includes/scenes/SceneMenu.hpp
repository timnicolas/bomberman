#pragma once

#include <vector>
#include <glm/glm.hpp>
#include "glm/gtx/string_cast.hpp"

#include "AScene.hpp"
#include "ABaseUI.hpp"

class SceneMenu : public AScene {
	public:
		// Members

		// Constructors
		explicit SceneMenu(Gui * gui);
		virtual ~SceneMenu();
		SceneMenu(SceneMenu const &src);

		// Operators
		SceneMenu &operator=(SceneMenu const &rhs);
		friend std::ostream& operator<<(std::ostream& os, const SceneMenu& my_class);

		// AScene methods
		virtual bool	init();
		virtual bool	update(std::chrono::milliseconds last_loop_ms);
		virtual bool	draw();

	protected:
		std::vector<ABaseUI *>	_buttons;
};
