#include "EnemyFly.hpp"
#include "Player.hpp"

// -- Constructors -------------------------------------------------------------

EnemyFly::EnemyFly(SceneGame &game)
: AEnemy(game)
{
	name = ENEMY_FLY_STR;
	size = glm::vec3(.8, .8, .8);
}

EnemyFly::~EnemyFly() {
	auto it = game.enemies.begin();
	while (it != game.enemies.end()) {
		if ((*it) == this)
			game.enemies.erase(it);
		else
			it++;
	}
}

EnemyFly::EnemyFly(EnemyFly const &src) : AEnemy(src) {
	*this = src;
}

// -- Operators ----------------------------------------------------------------

EnemyFly &EnemyFly::operator=(EnemyFly const &rhs) {
	if ( this != &rhs ) {
		AEnemy::operator=(rhs);
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
bool	EnemyFly::_update() {
	// update animation on state change
	_updateAnimationState();

	if (alive && _entityState.state != EntityState::ATTACK) {
		// do nothing if blocked
		if (_isBlocked()) {
			// update state on first stop
			if (_entityState.state != EntityState::IDLE) {
				setState(EntityState::IDLE);
			}
			return true;
		}

		_baseEnemyMove(_dir);
	}

	// update model pos/rot
	_updateModel();

	return true;
}

/**
 * @brief postUpdate is called each frame. After update()
 *
 * @return true if success
 * @return false if failure
 */
bool	EnemyFly::_postUpdate() {
	return true;
}

/**
 * @brief draw is called each frame.
 *
 * @return true if success
 * @return false if failure
 */
bool	EnemyFly::_draw(Gui &gui) {
	(void)gui;
	// gui.drawCube(Block::IA, getPos(), size);

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

/**
 * @brief update model position and rotation
 *
 */
void	EnemyFly::_updateModel() {
	_model->transform.setPos(position + glm::vec3(movingSize.x / 2, FLY_HEIGHT, movingSize.z / 2));

	// set model orientationdebug
	float	angle = glm::orientedAngle({0, 1}, glm::vec2(-front.x, front.z));
	_model->transform.setRot(angle);
}

/**
 * @brief update animation on state change
 *
 */
void	EnemyFly::_updateAnimationState() {
	if (_entityState.updated) {
		_entityState.updated = false;
		switch (_entityState.state) {
			case EntityState::IDLE:
				_model->animationSpeed = 1;
				_model->loopAnimation = true;
				// no idle animation, same as run (I Will create one if necessary)
				_model->setAnimation("Armature|run", &AEntity::animEndCb, this);
				break;
			case EntityState::DYING:
				_model->animationSpeed = 1;
				_model->loopAnimation = false;
				_model->setAnimation("Armature|death", &AEntity::animEndCb, this);
				break;
			case EntityState::RUNNING:
				_model->animationSpeed = 1;
				_model->loopAnimation = true;
				_model->setAnimation("Armature|run", &AEntity::animEndCb, this);
				break;
			case EntityState::ATTACK:
				_model->animationSpeed = 1;
				_model->loopAnimation = false;
				_model->setAnimation("Armature|attack", &AEntity::animEndCb, this);
				break;
			default:
				break;
		}
	}
}

/**
 * @brief Init enemyBasic
 *
 * @return true on success
 * @return false on failure
 */
bool	EnemyFly::init() {
	try {
		// if exist, delete last model
		if (_model)
			delete _model;

		OpenGLModel	&openglModel = ModelsManager::getModel("flyingBot");
		_model = new Model(openglModel, game.getDtTime(), ETransform({1.5,
			FLY_HEIGHT, 1.5}, ENEMY_FLY_SIZE));
		_model->play = true;
		_model->loopAnimation = true;
		_model->setAnimation("Armature|run");
		_updateModel();
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
