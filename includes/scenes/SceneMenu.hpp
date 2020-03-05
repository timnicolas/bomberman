#pragma once

#include <vector>
#include <glm/glm.hpp>
#include "glm/gtx/string_cast.hpp"

#include "AScene.hpp"
#include "ABaseUI.hpp"
#include "Button.hpp"
#include "Slider.hpp"

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

		Button *		addButton(glm::vec2 pos, glm::vec2 size, std::string const & text);
		Slider *		addSlider(glm::vec2 pos, glm::vec2 size, float min, float max, float val, float step = 0);
		ABaseUI *		getUIElement(uint32_t id);
		uint32_t		getNbUIElements() const;

	protected:
		std::vector<ABaseUI *>	_buttons;
};
