#include "Flag.hpp"
#include "SceneGame.hpp"
#include "AudioManager.hpp"

// -- Constructors -------------------------------------------------------------

Flag::Flag(SceneGame &game) : AObject(game) {
	type = Type::FLAG;
	name = FLAG_STR;
	destructible = true;
	blockPropagation = true;
	AudioManager::loadSound(FLAG_DESTROYED_SOUND);
	_soundOfDeath = FLAG_DESTROYED_SOUND;
}

Flag::~Flag() {
	if (game.clearFromBoard(this, {position.x, position.z})) {
		game.flags--;
	}
}

Flag::Flag(Flag const &src) : AObject(src) {
	*this = src;
}

// -- Operators ----------------------------------------------------------------

Flag &Flag::operator=(Flag const &rhs) {
	if ( this != &rhs ) {
		AObject::operator=(rhs);
	}
	return *this;
}

// -- Methods ------------------------------------------------------------------

/**
 * @brief Init Flag
 *
 * @return true on success
 * @return false on failure
 */
bool	Flag::init() {
	AObject::init();

	try {
		// if exist, delete last model
		if (_model)
			delete _model;

		_model = new Model(ModelsManager::getModel("server"), game.getDtTime(),
			ETransform((position + glm::vec3(.5, 0, .5))));
	}
	catch(ModelsManager::ModelsManagerException const &e) {
		logErr(e.what());
		return false;
	}
	catch(OpenGLModel::ModelException const &e) {
		logErr(e.what());
		return false;
	}

	return true;
}


/**
 * @brief update is called each frame.
 *
 * @return true if success
 * @return false if failure
 */
bool	Flag::update() {
	return true;
}

/**
 * @brief postUpdate is called each frame. After update.
 *
 * @return true if success
 * @return false if failure
 */
bool	Flag::postUpdate() {
	if (active == false || alive == false)
		delete this;
	return true;
}

/**
 * @brief draw is called each frame.
 *
 * @return true if success
 * @return false if failure
 */
bool	Flag::draw(Gui &gui) {
	(void)gui;
	// gui.drawCube(Block::FLAG, getPos());

	// draw model
	try {
		_model->draw();
	}
	catch(OpenGLModel::ModelException const & e) {
		logErr(e.what());
		return false;
	}

	return true;
}

// -- Exceptions errors --------------------------------------------------------

Flag::FlagException::FlagException()
: std::runtime_error("Flag Exception") {}

Flag::FlagException::FlagException(const char* whatArg)
: std::runtime_error(std::string(std::string("FlagError: ") + whatArg).c_str()) {}
