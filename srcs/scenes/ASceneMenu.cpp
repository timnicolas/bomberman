#include "ASceneMenu.hpp"

// -- Constructors -------------------------------------------------------------

ASceneMenu::ASceneMenu(Gui * gui, float const &dtTime)
: AScene(gui, dtTime) {}

ASceneMenu::~ASceneMenu() {
	for (auto it = _buttons.begin(); it != _buttons.end(); it++) {
		delete *it;
	}
	_buttons.clear();
}

ASceneMenu::ASceneMenu(ASceneMenu const &src)
: AScene(src) {
	*this = src;
}

// -- Operators ----------------------------------------------------------------

ASceneMenu &ASceneMenu::operator=(ASceneMenu const &rhs) {
	if ( this != &rhs ) {
		logWarn("ASceneMenu object copied");
	}
	return *this;
}

std::ostream &	operator<<(std::ostream & os, const ASceneMenu& myClass) {
	(void)myClass;
	os << "<ASceneMenu object>";
	return os;
}

// -- Methods ------------------------------------------------------------------

/**
 * @brief this is the update function (called every frames)
 *
 * @return true if the update is a success
 * @return false if there are an error in update
 */
bool	ASceneMenu::update() {
	for (auto it = _buttons.begin(); it != _buttons.end(); it++) {
		(*it)->update();
	}
	return true;
}

/**
 * @brief this is the draw function (called every frames)
 *
 * @return true if the draw is a success
 * @return false if there are an error in draw
 */
bool	ASceneMenu::draw() {
	for (auto it = _buttons.begin(); it != _buttons.end(); it++) {
		(*it)->draw();
	}
	return true;
}
/**
 * @brief called when the scene is loaded
 */
void ASceneMenu::load() {
	_gui->enableCursor(true);
}
/**
 * @brief called when the scene is unloaded
 */
void ASceneMenu::unload() {
}

/**
 * @brief add a button in the menu with menu settings
 *
 * @param pos the position
 * @param size the size
 * @param text the text in the button
 * @return ButtonUI& a reference to the element created
 */
ButtonUI & ASceneMenu::addButton(glm::vec2 pos, glm::vec2 size, std::string const & text) {
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
 * @return ButtonImageUI& a reference to the element created
 */
ButtonImageUI & ASceneMenu::addButtonImage(glm::vec2 pos, glm::vec2 size,
	std::string const & filename)
{
	ButtonImageUI * ui = new ButtonImageUI(pos, size, filename);
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
SliderUI & ASceneMenu::addSlider(glm::vec2 pos, glm::vec2 size, float min, float max, float val, float step) {
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
TextUI & ASceneMenu::addText(glm::vec2 pos, glm::vec2 size, std::string const & text) {
	TextUI * ui = new TextUI(pos, size);
	ui->setText(text);
	if (size == VOID_SIZE)
		ui->setCalculatedSize();
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
RectUI & ASceneMenu::addRect(glm::vec2 pos, glm::vec2 size, glm::vec4 color, glm::vec4 borderColor) {
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
 * @return ImageUI& a reference to the element created
 */
ImageUI & ASceneMenu::addImage(glm::vec2 pos, glm::vec2 size, std::string const & filename) {
	ImageUI * ui = new ImageUI(pos, size, filename);
	_buttons.push_back(ui);
	return *ui;
}

/**
 * @brief add a scrollbar in the menu with menu settings
 *
 * @param pos the position
 * @param size the size
 * @return ScrollbarUI& a reference to the element created
 */
ScrollbarUI & ASceneMenu::addScrollbar(glm::vec2 pos, glm::vec2 size) {
	ScrollbarUI * ui = new ScrollbarUI(pos, size);
	_buttons.push_back(ui);
	return *ui;
}

/**
 * @brief init the basic background of the menu
 *
 * @return true if success
 * @return false if error
 */
bool ASceneMenu::_initBG() {
	glm::vec2 winSz = _gui->gameInfo.windowSize;
	glm::vec2 tmpPos = glm::vec2(0, 0);
	glm::vec2 tmpSize = glm::vec2(200, 0);
	int i = 0;
	while (tmpPos.y < winSz.y) {
		tmpPos.x = 0;
		int j = 0;
		while (tmpPos.x < winSz.x) {
			std::string name;
			if ((i + j) & 1) name = "bomberman-assets/textures/bomb/005-bombFace.png";
			else name = "bomberman-assets/textures/player/009-playerFace.png";
			addImage(tmpPos, tmpSize, name).setColor(glm::vec4(1.0, 1.0, 1.0, 0.5));
			tmpPos.x += tmpSize.x;
			j++;
		}
		tmpPos.y += getUIElement(getNbUIElements() - 1).getSize().y;
		i++;
	}
	return true;
}

/* getter */
// get an UI element (button, slider, ...)
ABaseUI &		ASceneMenu::getUIElement(uint32_t id) { return *_buttons[id]; }
// get the total number of UI elements
uint32_t		ASceneMenu::getNbUIElements() const { return _buttons.size(); }
