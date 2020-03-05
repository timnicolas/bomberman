#include <unistd.h>
#include <stdlib.h>
#include <time.h>
// #include <bits/stdc++.h>

#include "SceneMenu.hpp"
#include "bomberman.hpp"
#include "Button.hpp"
#include "Inputs.hpp"

// -- Constructors -------------------------------------------------------------

SceneMenu::SceneMenu(Gui * gui)
: AScene(gui)
{
}

SceneMenu::~SceneMenu() {
}

SceneMenu::SceneMenu(SceneMenu const &src)
: AScene(src)
{
	*this = src;
}

// -- Operators ----------------------------------------------------------------

SceneMenu &SceneMenu::operator=(SceneMenu const &rhs) {
	if ( this != &rhs ) {
		logWarn("SceneMenu object copied");
	}
	return *this;
}

std::ostream &	operator<<(std::ostream & os, const SceneMenu& my_class) {
	(void)my_class;
	os << "<SceneMenu object>";
	return os;
}

// -- Methods ------------------------------------------------------------------

/**
 * init game method.
 */
bool			SceneMenu::init() {
	_gui->enableCursor(true);
	// TODO(tnicolas42) remove addButtons calls
	addButton(glm::vec2(150, 50), glm::vec2(150, 50), "text button")
		->setColor(glm::vec4(0.2, 0.1, 0.9, 1.0));
	addButton(glm::vec2(500, 500), glm::vec2(150, 50), "button 2")
		->setColor(glm::vec4(0.2, 0.1, 0.9, 1.0));
	return true;
}

/*
	call this function every frames to update the scene
*/
bool	SceneMenu::update(std::chrono::milliseconds last_loop_ms) {
	(void)last_loop_ms;
	for (auto it = _buttons.begin(); it != _buttons.end(); it++) {
		(*it)->update(Inputs::getMousePos(), MouseState::NO_CLICK);
	}
	return true;
}

/*
	call this function every frames to draw the scene
*/
bool	SceneMenu::draw() {
	for (auto it = _buttons.begin(); it != _buttons.end(); it++) {
		(*it)->draw();
	}
	return true;
}

/*
	add a button in the menu
	sceneobj->addButton(glm::vec2(100, 100), glm::vec2(150, 50), "text in button")
		->setColor(glm::vec4(0.2, 0.2, 0.8, 1.0));
*/
Button *		SceneMenu::addButton(glm::vec2 pos, glm::vec2 size, std::string const & text) {
	Button * b = new Button(_gui->gameInfo.windowSize, pos, size);
	b->setText(text);
	_buttons.push_back(b);
	return b;
}

/* getter */
// get an UI element (button, slider, ...)
ABaseUI *		SceneMenu::getUIElement(uint32_t id) { return _buttons[id]; }
// get the total number of UI elements
uint32_t		SceneMenu::getNbUIElements() const { return _buttons.size(); }
