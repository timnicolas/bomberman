#include "EnemyCrispy.hpp"
#include "Player.hpp"
#include "AudioManager.hpp"

// -- Constructors -------------------------------------------------------------

EnemyCrispy::EnemyCrispy(SceneGame &game)
: AEnemy(game),
  _isWall(true),
  _playerDir(Direction::NO_DIRECTION),
  _lastPayerSeenMs(0)
{
	name = ENEMY_CRISPY_STR;
	AudioManager::loadSound(ENEMY_CRISPY_DEATH_SOUND);
	_soundOfDeath = ENEMY_CRISPY_DEATH_SOUND;
}

EnemyCrispy::~EnemyCrispy() {
	auto it = game.enemies.begin();
	while (it != game.enemies.end()) {
		if ((*it) == this)
			game.enemies.erase(it);
		else
			it++;
	}
}

EnemyCrispy::EnemyCrispy(EnemyCrispy const &src) : AEnemy(src) {
	*this = src;
}

// -- Operators ----------------------------------------------------------------

EnemyCrispy &EnemyCrispy::operator=(EnemyCrispy const &rhs) {
	if ( this != &rhs ) {
		AEnemy::operator=(rhs);
		_isWall = rhs._isWall;
		_playerDir = rhs._playerDir;
		_lastPayerSeenMs = rhs._lastPayerSeenMs;
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
bool	EnemyCrispy::_update() {
	if (alive && _entityState.state != EntityState::ATTACK) {
		// do nothing if blocked
		if (_isBlocked()) {
			// update state on first stop
			if (_entityState.state != EntityState::IDLE) {
				setState(EntityState::IDLE);
			}
			return true;
		}

		/* retransform to a wall */
		if (getMs().count() - _lastPayerSeenMs > TIME_BEFORE_TRANSFORM_TO_WALL &&
			_isOn(getIntPos(), game.getDtTime() * speed * 3))
		{
			position.x = getIntPos().x;
			position.z = getIntPos().y;
			_isWall = true;
		}

		/* try to find player */
		Direction::Enum viewPlayerDir = _isPlayerVisible();
		if (viewPlayerDir != Direction::NO_DIRECTION) {
			_playerDir = viewPlayerDir;
			_dir = viewPlayerDir;
			_isWall = false;
			_lastPayerSeenMs = getMs().count();
		}

		/* move */
		if (!_isWall) {
			glm::vec3 tmpPos = position;
			if (tmpPos == _moveTo(_playerDir)) {
				_playerDir = Direction::NO_DIRECTION;
				_baseEnemyMove(_dir);
			}
		}
	}

	// update animation on state change
	_updateAnimationState();
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
bool	EnemyCrispy::_postUpdate() {
	return true;
}

/**
 * @brief draw is called each frame.
 *
 * @return true if success
 * @return false if failure
 */
bool	EnemyCrispy::_draw(Gui &gui) {
	(void)gui;
	// if (_isWall)
	// 	gui.drawCube(Block::DESTRUCTIBLE_WALL, getPos());
	// else
	// 	gui.drawCube(Block::IA, getPos(), size);

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
void	EnemyCrispy::_updateModel() {
	_model->transform.setPos(position + glm::vec3(movingSize.x / 2, 0, movingSize.z / 2));

	// set model orientationdebug
	float	angle = glm::orientedAngle({0, 1}, glm::vec2(-front.x, front.z));
	_model->transform.setRot(angle);
}

/**
 * @brief update animation on state change
 *
 */
void	EnemyCrispy::_updateAnimationState() {
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
				_model->animationSpeed = .7;
				_model->loopAnimation = true;
				_model->setAnimation("Armature|run", &AEntity::animEndCb, this);
				break;
			case EntityState::ATTACK:
				_model->animationSpeed = 2.3;
				_model->loopAnimation = false;
				_model->setAnimation("Armature|attack", &AEntity::animEndCb, this);
				break;
			default:
				break;
		}
	}
}

/**
 * @brief Init EnemyCrispy
 *
 * @return true on success
 * @return false on failure
 */
bool	EnemyCrispy::init() {
	try {
		// if exist, delete last model
		if (_model)
			delete _model;

		OpenGLModel	&openglModel = ModelsManager::getModel("crispy");
		_model = new Model(openglModel, game.getDtTime(), ETransform({0, 0, 0},
			ENEMY_CRISPY_SIZE));
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
