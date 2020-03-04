#include "Button.hpp"

Button::Button(glm::vec2 pos, glm::vec2 size)
: ABaseUI(pos, size)
{
}

Button::Button(Button const & src)
: ABaseUI(src)
{
	*this = src;
}

Button::~Button() {
}

Button & Button::operator=(Button const & rhs) {
	(void)rhs;
	// if (this != &rhs) {}
	return *this;
}

void Button::update(glm::vec2 mousePos, MouseState::ENUM mouseState) {
	(void)mousePos;
	(void)mouseState;
}

void Button::draw() {
}
