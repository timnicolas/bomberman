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
	for (auto it = _buttons.begin(); it != _buttons.end(); it++) {
		delete *it;
	}
	_buttons.clear();
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
	addButton(glm::vec2(150, 50), glm::vec2(150, 50), "text button");
	addButton(glm::vec2(500, 520), glm::vec2(250, 50), "button left")
		.setTextAlign(TextAlign::LEFT);
	addButton(glm::vec2(500, 460), glm::vec2(250, 50), "button center")
		.setTextAlign(TextAlign::CENTER);
	addButton(glm::vec2(500, 400), glm::vec2(250, 50), "button right")
		.setTextAlign(TextAlign::RIGHT);
	addSlider(glm::vec2(100, 500), glm::vec2(300, 100), 0, 128, 64, 4);
	return true;
}

/*
	call this function every frames to update the scene
*/
bool	SceneMenu::update(std::chrono::milliseconds last_loop_ms) {
	(void)last_loop_ms;
	for (auto it = _buttons.begin(); it != _buttons.end(); it++) {
		(*it)->update(Inputs::getMousePos(), Inputs::getRightClick(), Inputs::getLeftClick());
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
Button & SceneMenu::addButton(glm::vec2 pos, glm::vec2 size, std::string const & text) {
	Button * ui = new Button(_gui->gameInfo.windowSize, pos, size);
	ui->setText(text);
	// set color
	glm::vec4 color = glm::vec4(
		s.j("colors").j("buttons").d("r"),
		s.j("colors").j("buttons").d("g"),
		s.j("colors").j("buttons").d("b"),
		s.j("colors").j("buttons").d("a")
	);
	ui->setColor(color);
	_buttons.push_back(ui);
	return *ui;
}
Slider & SceneMenu::addSlider(glm::vec2 pos, glm::vec2 size, float min, float max, float val, float step) {
	Slider * ui = new Slider(_gui->gameInfo.windowSize, pos, size);
	ui->setValues(min, max, val, step);
	// set color
	glm::vec4 color = glm::vec4(
		s.j("colors").j("buttons").d("r"),
		s.j("colors").j("buttons").d("g"),
		s.j("colors").j("buttons").d("b"),
		s.j("colors").j("buttons").d("a")
	);
	ui->setColor(color);
	ui->setTextAlign(TextAlign::LEFT);
	_buttons.push_back(ui);
	return *ui;
}

/* getter */
// get an UI element (button, slider, ...)
ABaseUI &		SceneMenu::getUIElement(uint32_t id) { return *_buttons[id]; }
// get the total number of UI elements
uint32_t		SceneMenu::getNbUIElements() const { return _buttons.size(); }
