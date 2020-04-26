#include "EnemyWithEye.hpp"
#include "Player.hpp"
#include "AudioManager.hpp"

// -- Constructors -------------------------------------------------------------

EnemyWithEye::EnemyWithEye(SceneGame &game)
: AEnemy(game),
  _playerDir(Direction::NO_DIRECTION)
{
	name = ENEMY_WITH_EYE_STR;
	size = glm::vec3(0.7, 1.0, 0.7);
	AudioManager::loadSound(ENEMY_WITH_EYE_DEATH_SOUND);
	_soundOfDeath = ENEMY_WITH_EYE_DEATH_SOUND;
}

EnemyWithEye::~EnemyWithEye() {
}

EnemyWithEye::EnemyWithEye(EnemyWithEye const &src) : AEnemy(src) {
	*this = src;
}

// -- Operators ----------------------------------------------------------------

EnemyWithEye &EnemyWithEye::operator=(EnemyWithEye const &rhs) {
	if ( this != &rhs ) {
		AEnemy::operator=(rhs);
		_playerDir = rhs._playerDir;
	}
	return *this;
}

// -- Methods ------------------------------------------------------------------

/**
 * @brief Init enemyWithEye
 *
 * @return true on success
 * @return false on failure
 */
bool	EnemyWithEye::init() {
	try {
		// if exist, delete last model
		if (_model)
			delete _model;

		OpenGLModel	&openglModel = ModelsManager::getModel("flower");
		_model = new Model(openglModel, game.getDtTime(), ETransform({0, 0, 0}, ENEMY_WITH_EYE_SIZE));
		_model->play = true;
		_model->loopAnimation = true;
		_model->setAnimation("Armature|idle");
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

/**
 * @brief update is called each frame.
 *
 * @return true if success
 * @return false if failure
 */
bool	EnemyWithEye::_update() {
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

		Direction::Enum viewPlayerDir = _isPlayerVisible();
		if (viewPlayerDir != Direction::NO_DIRECTION) {
			_playerDir = viewPlayerDir;
			_dir = viewPlayerDir;
		}

		glm::vec3 tmpPos = position;
		if (tmpPos == _moveTo(_playerDir)) {
			_playerDir = Direction::NO_DIRECTION;
			_baseEnemyMove(_dir);
		}
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
bool	EnemyWithEye::_postUpdate() {
	return true;
}

/**
 * @brief draw is called each frame.
 *
 * @return true if success
 * @return false if failure
 */
bool	EnemyWithEye::_draw(Gui &gui) {
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
void	EnemyWithEye::_updateModel() {
	_model->transform.setPos(position + glm::vec3(movingSize.x / 2, 0, movingSize.z / 2));

	// set model orientation
	float	angle = glm::orientedAngle({0, 1}, glm::vec2(-front.x, front.z));
	_model->transform.setRot(angle);
}

/**
 * @brief update animation on state change
 *
 */
void	EnemyWithEye::_updateAnimationState() {
	if (_entityState.updated) {
		_entityState.updated = false;
		switch (_entityState.state) {
			case EntityState::IDLE:
				_model->animationSpeed = 1;
				_model->loopAnimation = true;
				_model->setAnimation("Armature|idle", &AEntity::animEndCb, this);
				break;
			case EntityState::DYING:
				_model->animationSpeed = 1;
				_model->loopAnimation = false;
				_model->setAnimation("Armature|death", &AEntity::animEndCb, this);
				break;
			case EntityState::RUNNING:
				_model->animationSpeed = .6;
				_model->loopAnimation = true;
				_model->setAnimation("Armature|run", &AEntity::animEndCb, this);
				break;
			case EntityState::ATTACK:
				_model->animationSpeed = 2;
				_model->loopAnimation = false;
				_model->setAnimation("Armature|attack", &AEntity::animEndCb, this);
				break;
			default:
				break;
		}
	}
}
