#include "EnemyFollow.hpp"
#include "Player.hpp"
#include "AudioManager.hpp"

// -- Constructors -------------------------------------------------------------

EnemyFollow::EnemyFollow(SceneGame &game)
: AEnemy(game),
  _findPlayer(false),
  _path()
{
	name = ENEMY_FOLLOW_STR;
	size = glm::vec3(0.8, 1.5, 0.8);
	_lastFindMs = getMs();
	AudioManager::loadSound(ENEMY_FOLLOW_DEATH_SOUND);
	_soundOfDeath = ENEMY_FOLLOW_DEATH_SOUND;
}

EnemyFollow::~EnemyFollow() {
}

EnemyFollow::EnemyFollow(EnemyFollow const &src) : AEnemy(src) {
	*this = src;
}

// -- Operators ----------------------------------------------------------------

EnemyFollow &EnemyFollow::operator=(EnemyFollow const &rhs) {
	if ( this != &rhs ) {
		_findPlayer = rhs._findPlayer;
		_path = rhs._path;
		_lastFindMs = rhs._lastFindMs;
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
bool	EnemyFollow::_update() {
	if (alive && _entityState.state != EntityState::ATTACK) {
		// do nothing if blocked
		if (_isBlocked()) {
			// update state on first stop
			if (_entityState.state != EntityState::IDLE) {
				setState(EntityState::IDLE);
			}
			return true;
		}

		// try to find a path to the player
		// after 1sec, 1 chance over 10 to relaunch path calculation
		if ((getMs() - _lastFindMs).count() > 1000 && (!_findPlayer || rand() % 100 < 10)) {
			_lastFindMs = getMs();
			_findPlayer = _getPathTo(game.player->getIntPos(), _path);
		}
		glm::vec3 lastPos = position;
		if (_path.size() == 0) {
			// arrived to destination
			_findPlayer = false;
		}
		else if (_followPath(_path) == false) {
			// blocked by a wall
			_findPlayer = false;
			_path.clear();
		}

		if (lastPos == position) {
			// if the enemy doesn't move
			_baseEnemyMove(_dir);
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
bool	EnemyFollow::_postUpdate() {
	return true;
}

/**
 * @brief draw is called each frame.
 *
 * @return true if success
 * @return false if failure
 */
bool	EnemyFollow::_draw(Gui &gui) {
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
void	EnemyFollow::_updateModel() {
	_model->transform.setPos(position + glm::vec3(movingSize.x / 2, 0, movingSize.z / 2));

	// set model orientationdebug
	float	angle = glm::orientedAngle({0, 1}, glm::vec2(-front.x, front.z));
	_model->transform.setRot(angle);
}

/**
 * @brief update animation on state change
 *
 */
void	EnemyFollow::_updateAnimationState() {
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
				_model->animationSpeed = 1.7;
				_model->loopAnimation = false;
				_model->setAnimation("Armature|attack", &AEntity::animEndCb, this);
				break;
			case EntityState::VICTORY_EMOTE:
				_model->animationSpeed = 1;
				_model->loopAnimation = true;
				_model->setAnimation("Armature|victory", &AEntity::animEndCb, this);
				break;
			default:
				break;
		}
	}
}

/**
 * @brief Init EnemyFollow
 *
 * @return true on success
 * @return false on failure
 */
bool	EnemyFollow::init() {
	try {
		// if exist, delete last model
		if (_model)
			delete _model;

		OpenGLModel	&openglModel = ModelsManager::getModel("follow");
		_model = new Model(openglModel, game.getDtTime(), ETransform({0, 0, 0}, ENEMY_FOLLOW_SIZE));
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
