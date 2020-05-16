#include "Fire.hpp"
#include "SceneGame.hpp"
#include "Player.hpp"
#include "AEnemy.hpp"

// -- Constructors -------------------------------------------------------------

Fire::Fire(SceneGame &game) : AObject(game) {
	type = Type::FIRE;
	name = "Fire";
	blockPropagation = false;
	destructible = true;
	_timeToDie = 1.0f;
}

Fire::~Fire() {
	game.clearFromBoard(this, {position.x, position.z});
}

Fire::Fire(Fire const &src) : AObject(src) {
	*this = src;
}

// -- Operators ----------------------------------------------------------------

Fire &Fire::operator=(Fire const &rhs) {
	if ( this != &rhs ) {
		AObject::operator=(rhs);
	}
	return *this;
}

// -- Methods ------------------------------------------------------------------


/**
 * @brief Init Fire
 *
 * @return true on success
 * @return false on failure
 */
bool	Fire::init() {
	AObject::init();

	try {
		// if exist, delete last model
		if (_model)
			delete _model;

		_model = new Model(ModelsManager::getModel("fire"), game.getDtTime(),
			ETransform((position + glm::vec3(.5, 0, .5))));
		_model->play = true;
		_model->loopAnimation = true;
		_model->setAnimation("Armature|idle");
		_model->animationSpeed = 2.8;
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
bool	Fire::update() {
	_timeToDie -= game.getDtTime();
	if (_timeToDie <= 0.0) {
		alive = false;
	}

	for (auto &&enemy : game.enemies) {
		if (enemy->hasCollision(position)) {
			enemy->takeDamage(1);
		}
	}

	if (game.player->hasCollision(position)) {
		if (!game.player->passFire)
			game.player->takeDamage(1);
	}

	return true;
}

bool	Fire::postUpdate() {
	if (!alive) {
		delete this;
	}
	return true;
}

/**
 * @brief draw is called each frame.
 *
 * @return true if success
 * @return false if failure
 */
bool	Fire::draw(Gui &gui) {
	(void)gui;
	// gui.drawCube(Block::FIRE, getPos());

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

Fire::FireException::FireException()
: std::runtime_error("Fire Exception") {}

Fire::FireException::FireException(const char* whatArg)
: std::runtime_error(std::string(std::string("FireError: ") + whatArg).c_str()) {}
