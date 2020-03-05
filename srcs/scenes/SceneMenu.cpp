#include <unistd.h>
#include <stdlib.h>
#include <time.h>
// #include <bits/stdc++.h>

#include "SceneMenu.hpp"
#include "bomberman.hpp"
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
	glm::vec2 winSz = _gui->gameInfo.windowSize;
	glm::vec2 tmpPos;
	glm::vec2 tmpSize;
	float menuWidth = winSz.x / 2;
	float menuHeight = menuWidth / 8;

	try {
		tmpPos.x = (winSz.x / 2) - (menuWidth / 2);
		tmpPos.y = winSz.y - menuHeight * 2;
		tmpSize.x = menuWidth;
		tmpSize.y = menuHeight;
		addText(tmpPos, tmpSize, "MENU").setTextScale(3);

		tmpPos.y -= menuHeight * 1.2;
		addButton(tmpPos, tmpSize, "button left").setTextAlign(TextAlign::LEFT);

		tmpPos.y -= menuHeight * 1.2;
		addButton(tmpPos, tmpSize, "button center").setTextAlign(TextAlign::CENTER);

		tmpPos.y -= menuHeight * 1.2;
		tmpSize.x = menuWidth / 3;
		addText(tmpPos, tmpSize, "sound level").setTextAlign(TextAlign::RIGHT);
		tmpPos.x += tmpSize.x;
		tmpSize.x = menuWidth / 3 * 2;
		addSlider(tmpPos, tmpSize, 0, 128, 64, 1);
		tmpPos.x = (winSz.x / 2) - (menuWidth / 2);
		tmpSize.x = menuWidth;

		tmpPos.y -= menuHeight * 1.2;
		tmpSize.x = menuWidth / 3;
		addText(tmpPos, tmpSize, "music level").setTextAlign(TextAlign::RIGHT);
		tmpPos.x += tmpSize.x;
		tmpSize.x = menuWidth / 3 * 2;
		addSlider(tmpPos, tmpSize, 0, 128, 64, 1);
		tmpPos.x = (winSz.x / 2) - (menuWidth / 2);
		tmpSize.x = menuWidth;

		tmpPos.y -= menuHeight * 1.2;
		addButton(tmpPos, tmpSize, "button right").setTextAlign(TextAlign::RIGHT);

		tmpSize.x = tmpSize.x * 1.2;
		tmpSize.y = winSz.y - tmpPos.y;
		tmpPos.x = (winSz.x / 2) - ((menuWidth * 1.2) / 2);
		tmpPos.y -= menuHeight * 0.5;
		addRect(tmpPos, tmpSize, glm::vec4(0.0, 0.0, 0.0, 0.0));

		tmpPos = glm::vec2(0, 0);
		tmpSize = glm::vec2(winSz.x / 5, 0);
		int i = 0;
		while (tmpPos.y < winSz.y) {
			tmpPos.x = 0;
			int j = 0;
			while (tmpPos.x < winSz.x) {
				std::string name;
				if ((i + j) & 1)	name = "bomberman-assets/textures/bomb/005-bombFace.png";
				else		name = "bomberman-assets/textures/player/009-playerFace.png";
				addImage(tmpPos, tmpSize, name, false).setColor(glm::vec4(1.0, 1.0, 1.0, 0.5));
				tmpPos.x += tmpSize.x;
				j++;
			}
			tmpPos.y += getUIElement(getNbUIElements() - 1).getSize().y;
			i++;
		}
	}
	catch (ABaseUI::UIException & e) {
		logErr(e.what());
		return false;
	}
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
	sceneobj->addButton(glm::vec2(100, 100), glm::vec2(150, 50), "text in button");

	in these functions, you can set the default settings for buttons / slider / ...

	@return a reference to the UI created
*/
ButtonUI & SceneMenu::addButton(glm::vec2 pos, glm::vec2 size, std::string const & text) {
	ButtonUI * ui = new ButtonUI(_gui->gameInfo.windowSize, pos, size);
	ui->setText(text);
	// set default color
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
SliderUI & SceneMenu::addSlider(glm::vec2 pos, glm::vec2 size, float min, float max, float val, float step) {
	SliderUI * ui = new SliderUI(_gui->gameInfo.windowSize, pos, size);
	ui->setValues(min, max, val, step);
	// set default color
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
TextUI & SceneMenu::addText(glm::vec2 pos, glm::vec2 size, std::string const & text) {
	TextUI * ui = new TextUI(_gui->gameInfo.windowSize, pos, size);
	ui->setText(text);
	_buttons.push_back(ui);
	return *ui;
}
RectUI & SceneMenu::addRect(glm::vec2 pos, glm::vec2 size, glm::vec4 color, glm::vec4 borderColor) {
	RectUI * ui = new RectUI(_gui->gameInfo.windowSize, pos, size);
	ui->setColor(color);
	ui->setBorderColor(borderColor);
	_buttons.push_back(ui);
	return *ui;
}
ImageUI & SceneMenu::addImage(glm::vec2 pos, glm::vec2 size, std::string const & filename, bool pixelateOnZoom) {
	ImageUI * ui = new ImageUI(_gui->gameInfo.windowSize, pos, size, filename, pixelateOnZoom);
	_buttons.push_back(ui);
	return *ui;
}

/* getter */
// get an UI element (button, slider, ...)
ABaseUI &		SceneMenu::getUIElement(uint32_t id) { return *_buttons[id]; }
// get the total number of UI elements
uint32_t		SceneMenu::getNbUIElements() const { return _buttons.size(); }
