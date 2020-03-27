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
	if (!_initDone) {
		_initDone = true;

		try {
			_models["white"] = new OpenGLModel(cam, "bomberman-assets/3dModels/"
				"white/white_with_anims.fbx", {0, -0.5, 0});
		}
		catch(OpenGLModel::ModelException const & e) {
			logErr(e.what());
			return false;
		}
	}
	return true;
}
