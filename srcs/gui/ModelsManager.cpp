#include "ModelsManager.hpp"

#include "Logging.hpp"

// -- Constructors -------------------------------------------------------------
ModelsManager::ModelsManager()
: _initDone(false) {
}

ModelsManager::~ModelsManager() {
	// remove all models
	for (auto it = _models.cbegin(); it != _models.cend(); ) {
		delete it->second;
		it = _models.erase(it);
	}
}

ModelsManager::ModelsManager(ModelsManager const &src) {
	*this = src;
}

ModelsManager &ModelsManager::operator=(ModelsManager const &rhs) {
	if (this != &rhs) {
		logWarn("ModelsManager singleton should not be copied")
	}
	return *this;
}

// -- get ----------------------------------------------------------------------
/**
 * @brief Return a reference to the singleton AudioManager.
 *
 * @return the reference to the singleton.
 */
ModelsManager	&ModelsManager::get() {
	static ModelsManager	instance;
	return (instance);
}

// -- init ----------------------------------------------------------------------
/**
 * @brief load all models
 *
 * @param cam Camera reference
 * @return true on success
 * @return false on failure
 */
bool	ModelsManager::init(Camera const &cam) {
	return get()._init(cam);
}


bool	ModelsManager::_init(Camera const &cam) {
	if (!_initDone) {
		_initDone = true;

		try {
			_models["white"] = new OpenGLModel(cam, "bomberman-assets/3dModels/"
				"white/white_with_anims.fbx");

			_models["flower"] = new OpenGLModel(cam, "bomberman-assets/3dModels/"
				"flower/flower_with_anims.fbx");
		}
		catch(OpenGLModel::ModelException const & e) {
			logErr(e.what());
			return false;
		}
	}
	return true;
}

// -- init ----------------------------------------------------------------------
/**
 * @brief Get the Model object
 *
 * @param name
 * @return OpenGLModel&
 */
OpenGLModel	&ModelsManager::getModel(std::string const &name) {
	return get()._getModel(name);
}

OpenGLModel	&ModelsManager::_getModel(std::string const &name) {
	auto	it = _models.find(name);

	if (it != _models.end()) {
		return *(it->second);
	}

	throw ModelsManagerException(std::string("model \"" + name + "\" not found").c_str());
}

// -- exceptions ---------------------------------------------------------------
ModelsManager::ModelsManagerException::ModelsManagerException()
: std::runtime_error("[ModelsManagerException]") {}

ModelsManager::ModelsManagerException::ModelsManagerException(const char* what_arg)
: std::runtime_error(std::string(std::string("[ModelsManagerException] ") +
	what_arg).c_str()) {}
