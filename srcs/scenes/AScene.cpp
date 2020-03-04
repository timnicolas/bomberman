#include "AScene.hpp"

AScene::AScene(Gui *gui)
: _gui(gui)
{
}

AScene::AScene(AScene const & src) {
	*this = src;
}

AScene::~AScene() {
	_gui->enableCursor(true);
}

AScene & AScene::operator=(AScene const & rhs) {
	if (this != &rhs) {
		logWarn("scene object copied");
		_gui = rhs._gui;
	}
	return *this;
}

// -- Exceptions errors --------------------------------------------------------

AScene::SceneException::SceneException()
: std::runtime_error("SceneException") {}

AScene::SceneException::SceneException(const char* what_arg)
: std::runtime_error(std::string(std::string("SceneError: ") + what_arg).c_str()) {}
