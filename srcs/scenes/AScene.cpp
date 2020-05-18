#include "AScene.hpp"

/**
 * @brief Construct a new AScene::AScene object
 *
 * @param gui A pointer on the gui object
 * @param dtTime A reference to the delta time
 */
AScene::AScene(Gui *gui, float const &dtTime)
: _gui(gui),
  _dtTime(dtTime) {}

/**
 * @brief Construct a new AScene::AScene object
 *
 * @param src The object to do the copy
 */
AScene::AScene(AScene const & src)
: _gui(src._gui),
  _dtTime(src._dtTime) {
	*this = src;
}

/**
 * @brief Destroy the AScene::AScene object
 */
AScene::~AScene() {
	_gui->enableCursor(true);
}

/**
 * @brief Copy this object
 *
 * @param rhs The opjet to copy
 * @return AScene& A reference to the copied object
 */
AScene & AScene::operator=(AScene const & rhs) {
	if (this != &rhs) {
		logWarn("scene object copied");
		_gui = rhs._gui;
	}
	return *this;
}

// -- getters --------------------------------------------------------
/**
 * @brief Get the delta time
 *
 * @return float const& The delta time
 */
float const	&AScene::getDtTime() const { return _dtTime; }


// -- Exceptions errors --------------------------------------------------------
/**
 * @brief Construct a new AScene::SceneException::SceneException object
 */
AScene::SceneException::SceneException()
: std::runtime_error("SceneException") {}

/**
 * @brief Construct a new AScene::SceneException::SceneException object
 *
 * @param whatArg Error message
 */
AScene::SceneException::SceneException(const char* whatArg)
: std::runtime_error(std::string(std::string("SceneError: ") + whatArg).c_str()) {}
