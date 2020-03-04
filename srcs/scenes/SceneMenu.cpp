#include <unistd.h>
#include <stdlib.h>
#include <time.h>
// #include <bits/stdc++.h>

#include "SceneMenu.hpp"
#include "bomberman.hpp"

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
	return true;
}

// -- Private Methods ----------------------------------------------------------

bool	SceneMenu::update(std::chrono::milliseconds last_loop_ms) {
	(void)last_loop_ms;
	return true;
}

bool	SceneMenu::draw() {
	return true;
}
