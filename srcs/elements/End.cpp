#include "End.hpp"
#include "SceneGame.hpp"
#include "Player.hpp"
#include "AudioManager.hpp"

// -- Constructors -------------------------------------------------------------

End::End(SceneGame &game) : AObject(game) {
	type = Type::END;
	name = END_STR;
	_texture = Block::END;
	AudioManager::loadSound(END_OPEN_SOUND);
	_open = false;
	_model2 = nullptr;
}

End::~End() {
	delete _model2;
}

End::End(End const &src) : AObject(src) {
	_model2 = nullptr;
	*this = src;
}

// -- Operators ----------------------------------------------------------------

End &End::operator=(End const &rhs) {
	if (this != &rhs) {
		AObject::operator=(rhs);
		_texture = rhs._texture;
	}
	return *this;
}

// -- Methods ------------------------------------------------------------------

/**
 * @brief Init End
 *
 * @return true on success
 * @return false on failure
 */
bool	End::init() {
	AObject::init();

	try {
		// if exist, delete last model
		if (_model)
			delete _model;

		_model = new Model(ModelsManager::getModel("exit_base"), game.getDtTime(),
			ETransform((position + glm::vec3(.5, 0, .5))));

		if (_model2)
			delete _model2;

		_model2 = new Model(ModelsManager::getModel("exit_shield"), game.getDtTime(),
			ETransform((position + glm::vec3(.5, 0, .5))));
		_model2->play = true;
		_model2->loopAnimation = true;
		_model2->setAnimation("Armature|idle");
		_model2->animationSpeed = .25;
		// set shield texture to close
		_model2->setMeshTexture(TextureType::DIFFUSE, "Shield.001::Tube",
			CLOSE_PORTAL_TEXTURE);
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
bool	End::update() {
	if (game.flags <= 0) {
		if (game.enemiesToKill <= game.enemiesKilled) {
			// update open status
			if (!_open) {
				_texture = Block::END_OPEN;
				try {
					AudioManager::playSound(END_OPEN_SOUND);
				} catch(Sound::SoundException const & e) {
					logErr(e.what());
				}
				_open = true;

				// set shield texture to open
				_model2->setMeshTexture(TextureType::DIFFUSE, "Shield.001::Tube",
					OPEN_PORTAL_TEXTURE);
			}

			// add the End block to the Player colision list
			if (std::find(game.player->crossableTypes.begin(), game.player->crossableTypes.end(), Type::END)
			== game.player->crossableTypes.end())
			{
				game.player->crossableTypes.push_back(Type::END);
			}

			// if we detect a collision with the player set game state to Win
			std::unordered_set<AEntity *> collisions = _getCollision();
			for (auto &&entity : collisions) {
				if (entity->type == Type::PLAYER) {
					game.state = GameState::WIN;
				}
			}
		}
	}
	return true;
}

/**
 * @brief draw is called each frame.
 *
 * @return true if success
 * @return false if failure
 */
bool	End::draw(Gui &gui) {
	(void)gui;
	// gui.drawCube(_texture, getPos());

	// draw model
	try {
		_model->draw();
		_model2->draw();
	}
	catch(OpenGLModel::ModelException const & e) {
		logErr(e.what());
		return false;
	}

	return true;
}

// -- Private Methods ----------------------------------------------------------

/**
 * @brief get a list of entity in collision with the End.
 *
 * @return std::unordered_set<AEntity *> collisions
 */
std::unordered_set<AEntity *>	End::_getCollision() {
	std::unordered_set<AEntity *> collisions;

	if (game.player->hasCollision(position))
		collisions.insert(game.player);

	return collisions;
}

// -- Exceptions errors --------------------------------------------------------

End::EndException::EndException()
: std::runtime_error("End Exception") {}

End::EndException::EndException(const char* whatArg)
: std::runtime_error(std::string(std::string("EndError: ") + whatArg).c_str()) {}
