#include <unistd.h>
#include <stdlib.h>
#include <time.h>
// #include <bits/stdc++.h>

#include "SceneMenu.hpp"
#include "bomberman.hpp"
#include "Inputs.hpp"

// -- Constructors -------------------------------------------------------------

SceneMenu::SceneMenu(Gui * gui, float const &dtTime)
: AScene(gui, dtTime) {}

SceneMenu::~SceneMenu() {
	for (auto it = _buttons.begin(); it != _buttons.end(); it++) {
		delete *it;
	}
	_buttons.clear();
}

SceneMenu::SceneMenu(SceneMenu const &src)
: AScene(src) {
	*this = src;
}

// -- Operators ----------------------------------------------------------------

SceneMenu &SceneMenu::operator=(SceneMenu const &rhs) {
	if ( this != &rhs ) {
		logWarn("SceneMenu object copied");
	}
	return *this;
}

std::ostream &	operator<<(std::ostream & os, const SceneMenu& myClass) {
	(void)myClass;
	os << "<SceneMenu object>";
	return os;
}

// -- Methods ------------------------------------------------------------------

/**
 * @brief init the menu
 *
 * @return true if the init succed
 * @return false if the init failed
 */
bool			SceneMenu::init() {
	glm::vec2 winSz = _gui->gameInfo.windowSize;
	glm::vec2 tmpPos;
	glm::vec2 tmpSize;
	float menuWidth = winSz.x / 2;
	float menuHeight = menuWidth / 8;

	try {
		ABaseUI::loadFont("title", s.j("font").s("file"), s.j("font").u("size") * 3);

		tmpPos.x = (winSz.x / 2) - (menuWidth / 2);
		tmpPos.y = winSz.y - menuHeight * 2;
		tmpSize.x = menuWidth;
		tmpSize.y = menuHeight;
		addText(tmpPos, tmpSize, "MENU").setTextFont("title");

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

		tmpPos.y -= menuHeight * 1.2;
		tmpSize.x = 0;
		addButtonImage(tmpPos, tmpSize, "bomberman-assets/textures/player/011-playerTop.png");
		tmpSize.x = menuWidth;

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

/**
 * @brief this is the update function (called every frames)
 *
 * @return true if the update is a success
 * @return false if there are an error in update
 */
bool	SceneMenu::update() {
	for (auto it = _buttons.begin(); it != _buttons.end(); it++) {
		(*it)->update(Inputs::getMousePos(), Inputs::getRightClick(), Inputs::getLeftClick());
	}
	return true;
}

/**
 * @brief this is the draw function (called every frames)
 *
 * @return true if the draw is a success
 * @return false if there are an error in draw
 */
bool	SceneMenu::draw() {
	for (auto it = _buttons.begin(); it != _buttons.end(); it++) {
		(*it)->draw();
	}
	return true;
}
/**
 * @brief called when the scene is loaded
 *
 */
void SceneMenu::load() {
	_gui->enableCursor(true);
}
/**
 * @brief called when the scene is unloaded
 *
 */
void SceneMenu::unload() {
}

/**
 * @brief add a button in the menu with menu settings
 *
 * @param pos the position
 * @param size the size
 * @param text the text in the button
 * @return ButtonUI& a reference to the element created
 */
ButtonUI & SceneMenu::addButton(glm::vec2 pos, glm::vec2 size, std::string const & text) {
	ButtonUI * ui = new ButtonUI(pos, size);
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

/**
 * @brief add a button with image in the menu with menu settings
 *
 * @param pos the position
 * @param size the size
 * @param filename the path to the image
 * @param pixelateOnZoom a bool to enable/disable pixelate on zoom option
 * @return ButtonImageUI& a reference to the element created
 */
ButtonImageUI & SceneMenu::addButtonImage(glm::vec2 pos, glm::vec2 size, std::string const & filename,
bool pixelateOnZoom)
{
	ButtonImageUI * ui = new ButtonImageUI(pos, size, filename, pixelateOnZoom);
	_buttons.push_back(ui);
	return *ui;
}

/**
 * @brief add a slider in the menu with menu settings
 *
 * @param pos the position
 * @param size the size
 * @param min min value in slider
 * @param max max value in slider
 * @param val default value in slider
 * @param step step of the slider
 * @return SliderUI& a reference to the element created
 */
SliderUI & SceneMenu::addSlider(glm::vec2 pos, glm::vec2 size, float min, float max, float val, float step) {
	SliderUI * ui = new SliderUI(pos, size);
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

/**
 * @brief add a text in the menu with menu settings
 *
 * @param pos the position
 * @param size the size
 * @param text the text
 * @return TextUI& a reference to the element created
 */
TextUI & SceneMenu::addText(glm::vec2 pos, glm::vec2 size, std::string const & text) {
	TextUI * ui = new TextUI(pos, size);
	ui->setText(text);
	_buttons.push_back(ui);
	return *ui;
}

/**
 * @brief add a rectange in the menu with menu settings
 *
 * @param pos the position
 * @param size the size
 * @param color the rectangle color
 * @param borderColor the border rectangle color
 * @return RectUI& a reference to the element created
 */
RectUI & SceneMenu::addRect(glm::vec2 pos, glm::vec2 size, glm::vec4 color, glm::vec4 borderColor) {
	RectUI * ui = new RectUI(pos, size);
	ui->setColor(color);
	ui->setBorderColor(borderColor);
	_buttons.push_back(ui);
	return *ui;
}

/**
 * @brief add an image in the menu with menu settings
 *
 * @param pos the position
 * @param size the size
 * @param filename the path to the image
 * @param pixelateOnZoom a bool to enable/disable pixelate on zoom option
 * @return ImageUI& a reference to the element created
 */
ImageUI & SceneMenu::addImage(glm::vec2 pos, glm::vec2 size, std::string const & filename, bool pixelateOnZoom) {
	ImageUI * ui = new ImageUI(pos, size, filename, pixelateOnZoom);
	_buttons.push_back(ui);
	return *ui;
}

/* getter */
// get an UI element (button, slider, ...)
ABaseUI &		SceneMenu::getUIElement(uint32_t id) { return *_buttons[id]; }
// get the total number of UI elements
uint32_t		SceneMenu::getNbUIElements() const { return _buttons.size(); }
