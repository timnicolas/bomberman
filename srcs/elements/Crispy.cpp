#include "Crispy.hpp"

#include <cstdlib>

#include "SceneGame.hpp"
#include "Player.hpp"
#include "Bonus.hpp"

// -- Constructors -------------------------------------------------------------

/**
 * @brief Construct a new Crispy:: Crispy object
 *
 * @param game A reference to the SceneGame master object
 */
Crispy::Crispy(SceneGame &game) : AObject(game) {
	type = Type::CRISPY;
	name = CRISPY_STR;
	destructible = true;
}

/**
 * @brief Destroy the Crispy:: Crispy object
 */
Crispy::~Crispy() {
	game.clearFromBoard(this, {position.x, position.z});
}

/**
 * @brief Construct a new Crispy:: Crispy object
 *
 * @param src The object to do the copy
 */
Crispy::Crispy(Crispy const &src) : AObject(src) {
	*this = src;
}

// -- Operators ----------------------------------------------------------------

/**
 * @brief Copy this object
 *
 * @param rhs The opjet to copy
 * @return Crispy& A reference to the copied object
 */
Crispy &Crispy::operator=(Crispy const &rhs) {
	if ( this != &rhs ) {
		AObject::operator=(rhs);
	}
	return *this;
}

// -- Methods ------------------------------------------------------------------

/**
 * @brief update is called each frame.
 *
 * @return true if success
 * @return false if failure
 */
bool	Crispy::update() {
	if (!alive)
		active = false;
	return true;
}

bool	Crispy::postUpdate() {
	if (!active) {
		Bonus	*bonus = Bonus::generateBonus(game, .3f);
		if (bonus != nullptr) {
			game.board[position.x][position.z].push_back(bonus);
			bonus->init();
		}
		delete this;
		return false;
	}
	return true;
}

/**
 * @brief draw is called each frame.
 *
 * @return true if success
 * @return false if failure
 */
bool	Crispy::draw(Gui &gui) {
	(void)gui;
	// gui.drawCube(Block::DESTRUCTIBLE_WALL, getPos());

	// draw model
	try {
		_model->draw();
	}
	catch(OpenGLModel::ModelException const & e) {
		logErr(e.what());
		return false;
	}

	// TODO(zer0nim): create explode animation
	// _updateAnimationState();

	return true;
}

/**
 * @brief Generate a crispy wall with a certain chance
 *
 * @param game A reference to the main game object
 * @param genWallPercent chance to generate a wall [0:100]
 * @return Crispy* The new crispy wall or a nullptr
 */
Crispy * Crispy::generateCrispy(SceneGame &game, uint32_t genWallPercent) {
	if (genWallPercent <= 0)
		return nullptr;
	if (genWallPercent >= 100)
		return new Crispy(game);

	if (static_cast<uint32_t>(rand() % 100) > genWallPercent)
		return nullptr;
	return new Crispy(game);
}

/**
 * @brief update animation on state change
 *
 */
void	Crispy::_updateAnimationState() {
	if (_entityState.updated) {
		_entityState.updated = false;
		switch (_entityState.state) {
			case EntityState::DYING:
				_model->play = true;
				_model->animationSpeed = 1;
				_model->loopAnimation = false;
				_model->setAnimation("Armature|death", &AEntity::animEndCb, this);
				break;
			default:
				break;
		}
	}
}

/**
 * @brief Init Crispy
 *
 * @return true on success
 * @return false on failure
 */
bool	Crispy::init() {
	AObject::init();

	try {
		// if exist, delete last model
		if (_model)
			delete _model;

		OpenGLModel	&openglModel = ModelsManager::getModel("crispy");
		_model = new Model(openglModel, game.getDtTime(), ETransform(
			(position + glm::vec3(.5, 0, .5)),
			ENEMY_CRISPY_SIZE));

		// set random orientation
		float angles[] = {0, 90, 180, 270};
		float	angle = glm::radians(angles[rand() % 4]);
		_model->transform.setRot(angle);
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

// -- Exceptions errors --------------------------------------------------------

Crispy::CrispyException::CrispyException()
: std::runtime_error("Crispy Exception") {}

Crispy::CrispyException::CrispyException(const char* whatArg)
: std::runtime_error(std::string(std::string("CrispyError: ") + whatArg).c_str()) {}
