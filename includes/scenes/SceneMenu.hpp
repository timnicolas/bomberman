#pragma once

#include <vector>
#include "useGlm.hpp"

#include "AScene.hpp"
#include "ABaseUI.hpp"
#include "ButtonUI.hpp"
#include "SliderUI.hpp"
#include "TextUI.hpp"
#include "RectUI.hpp"
#include "ImageUI.hpp"

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

		/* add element */
		ButtonUI &		addButton(glm::vec2 pos, glm::vec2 size, std::string const & text);
		SliderUI &		addSlider(glm::vec2 pos, glm::vec2 size, float min, float max, float val, float step = 0);
		TextUI &		addText(glm::vec2 pos, glm::vec2 size, std::string const & text);
		RectUI &		addRect(glm::vec2 pos, glm::vec2 size, glm::vec4 color,
			glm::vec4 borderColor = glm::vec4(0.0, 0.0, 0.0, 1.0));
		ImageUI &		addImage(glm::vec2 pos, glm::vec2 size, std::string const & filename, bool pixelateOnZoom = true);

		/* getter */
		ABaseUI &		getUIElement(uint32_t id);
		uint32_t		getNbUIElements() const;

	protected:
		std::vector<ABaseUI *>	_buttons;
};
