#include "AScene.hpp"

AScene::AScene(Gui *gui, float const &dtTime)
: _gui(gui),
  _dtTime(dtTime) {}

AScene::AScene(AScene const & src)
: _gui(src._gui),
  _dtTime(src._dtTime) {
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

// -- getters --------------------------------------------------------
float const	&AScene::getDtTime() const { return _dtTime; }


// -- Exceptions errors --------------------------------------------------------

AScene::SceneException::SceneException()
: std::runtime_error("SceneException") {}

AScene::SceneException::SceneException(const char* whatArg)
: std::runtime_error(std::string(std::string("SceneError: ") + whatArg).c_str()) {}
