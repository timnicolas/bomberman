#pragma once

#include <vector>
#include "useGlm.hpp"

#include "AScene.hpp"
#include "SceneManager.hpp"

/* import all UI objects */
#include "ABaseUI.hpp"
#include "ButtonUI.hpp"
#include "ButtonImageUI.hpp"
#include "SliderUI.hpp"
#include "TextUI.hpp"
#include "RectUI.hpp"
#include "ImageUI.hpp"

/**
 * @brief Scene object to reimplement in all scenes for menu
 *
 * this object contains functions to create buttons, images, ...
 */
class ASceneMenu : public AScene {
	public:
		// Members

		// Constructors
		ASceneMenu(Gui * gui, float const &dtTime);
		virtual ~ASceneMenu();
		ASceneMenu(ASceneMenu const &src);

		// Operators
		ASceneMenu &operator=(ASceneMenu const &rhs);
		friend std::ostream& operator<<(std::ostream& os, const ASceneMenu& myClass);

		// AScene methods
		virtual bool		init() = 0;
		virtual bool		update();
		virtual bool		draw();
		virtual void		load();
		virtual void		unload();

		/* add element */
		ButtonUI &			addButton(glm::vec2 pos, glm::vec2 size, std::string const & text);
		ButtonImageUI &		addButtonImage(glm::vec2 pos, glm::vec2 size, std::string const & filename,
			bool pixelateOnZoom = true);
		SliderUI &			addSlider(glm::vec2 pos, glm::vec2 size, float min, float max, float val, float step);
		TextUI &			addText(glm::vec2 pos, glm::vec2 size, std::string const & text);
		RectUI &			addRect(glm::vec2 pos, glm::vec2 size, glm::vec4 color,
			glm::vec4 borderColor = glm::vec4(0.0, 0.0, 0.0, 1.0));
		ImageUI &			addImage(glm::vec2 pos, glm::vec2 size, std::string const & filename,
			bool pixelateOnZoom = true);

		/* getter */
		ABaseUI &			getUIElement(uint32_t id);
		uint32_t			getNbUIElements() const;

	protected:
		std::vector<ABaseUI *>	_buttons;
};
