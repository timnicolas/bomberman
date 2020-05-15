#include "EnemyFrog.hpp"
#include "Player.hpp"
#include "AudioManager.hpp"

// -- Constructors -------------------------------------------------------------

/**
 * @brief Construct a new Enemy Frog:: Enemy Frog object
 *
 * @param game A reference to the SceneGame master object
 */
EnemyFrog::EnemyFrog(SceneGame &game)
: AEnemy(game),
  _jumpGoal(VOID_POS),
  _nextJumpTime(0)
{
	name = ENEMY_FROG_STR;
	size = glm::vec3(0.7, 0.5, 0.7);
	strength = 0;  // remove auto damage
	resetCrossable();
	AudioManager::loadSound(ENEMY_FROG_DEATH_SOUND);
	_soundOfDeath = ENEMY_FROG_DEATH_SOUND;
}

/**
 * @brief Destroy the Enemy Frog:: Enemy Frog object
 */
EnemyFrog::~EnemyFrog() {
	auto it = game.enemies.begin();
	while (it != game.enemies.end()) {
		if ((*it) == this)
			game.enemies.erase(it);
		else
			it++;
	}
}

/**
 * @brief Construct a new Enemy Frog:: Enemy Frog object
 *
 * @param src The object to do the copy
 */
EnemyFrog::EnemyFrog(EnemyFrog const &src) : AEnemy(src) {
	*this = src;
}

// -- Operators ----------------------------------------------------------------

/**
 * @brief Copy this object
 *
 * @param rhs The opjet to copy
 * @return EnemyFrog& A reference to the copied object
 */
EnemyFrog &EnemyFrog::operator=(EnemyFrog const &rhs) {
	if ( this != &rhs ) {
		AEnemy::operator=(rhs);
		_jumpGoal = rhs._jumpGoal;
		_nextJumpTime = rhs._nextJumpTime;
	}
	return *this;
}

// -- Methods ------------------------------------------------------------------

/**
 * @brief Set the Entity that the Character can cross
 */
void EnemyFrog::resetCrossable() {
	ACharacter::resetCrossable();
	crossableTypes.clear();
	crossableTypes.push_back(Type::FIRE);
	crossableTypes.push_back(Type::BONUS);
}

/**
 * @brief update is called each frame.
 *
 * @return true if success
 * @return false if failure
 */
bool	EnemyFrog::_update() {
	if (alive) {
		/* try jumping */
		if (_jumpGoal == VOID_POS && static_cast<uint64_t>(getMs().count()) >= _nextJumpTime) {
			// try to find a new destination
			_findJumpGoal();

			// If there is no possible destinations
			if (_jumpGoal == VOID_POS) {
				// always reset chrono (don't retry in the next update if there is no move options)
				_nextJumpTime = getMs().count() + WAIT_JUMP_TIME_MS;
			}
		}

		/* moving */
		// if jumping
		if (_jumpGoal != VOID_POS) {
			_jumpCrossable();
			if (_isOn(_jumpGoal, game.getDtTime() * speed * 3)) {  // end of the jump
				// set position with precision
				position.x = _jumpGoal.x;
				position.z = _jumpGoal.y;
				_jumpGoal = VOID_POS;

				// long or short wait
				if (rand() % CHANCE_LONG_WAIT == 0)
					_nextJumpTime = getMs().count() + LONG_WAIT_JUMP_TIME_MS;
				else
					_nextJumpTime = getMs().count() + WAIT_JUMP_TIME_MS;
			}
			_moveTo(_dir, -1);
		}
		else {  // if stay in a position
			resetCrossable();
			if (game.player->hasCollision(position, size)) {
				game.player->takeDamage(1);
			}

			if (_entityState.state != EntityState::IDLE) {
				setState(EntityState::IDLE);
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
 * @brief Try to find a goal to jump on (This function can update _dir & _jumpGoal)
 */
void	EnemyFrog::_findJumpGoal() {
	int order = rand() % 3;  // random the order of elements (except go back option)
	Direction::Enum tryDirOrder[4] = {
		static_cast<Direction::Enum>((_dir + (order == 0 ? 0 : (order == 1 ? 1 : 3))) % 4),
		static_cast<Direction::Enum>((_dir + (order == 0 ? 3 : (order == 1 ? 1 : 0))) % 4),
		static_cast<Direction::Enum>((_dir + (order == 0 ? 1 : (order == 1 ? 3 : 0))) % 4),
		static_cast<Direction::Enum>((_dir + 2) % 4),
	};

	glm::ivec2 ipos = getIntPos();
	glm::ivec2 nextPos[4] = {
		{ 0, -FROG_JUMP_DIST},  // UP
		{ FROG_JUMP_DIST,  0},  // RIGHT
		{ 0,  FROG_JUMP_DIST},  // DOWN
		{-FROG_JUMP_DIST,  0},  // LEFT
	};

	// try all directions
	for (int i = 0; i < 4; i++) {
		glm::ivec2 tmpPos(ipos.x + nextPos[tryDirOrder[i]].x, ipos.y + nextPos[tryDirOrder[i]].y);
		if (_canWalkOnBlock(tmpPos)) {
			// start a jump
			_dir = tryDirOrder[i];
			_jumpGoal = tmpPos;
			break;
		}
	}
}

/**
 * @brief postUpdate is called each frame. After update()
 *
 * @return true if success
 * @return false if failure
 */
bool	EnemyFrog::_postUpdate() {
	return true;
}

/**
 * @brief draw is called each frame.
 *
 * @return true if success
 * @return false if failure
 */
bool	EnemyFrog::_draw(Gui &gui) {
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
 * @brief Change crossable collision durring jump to allow to Frog to pass throw
 *
 */
void	EnemyFrog::_jumpCrossable() {
	ACharacter::resetCrossable();
	crossableTypes.clear();
	crossableTypes.push_back(Type::ALL);
}

/**
 * @brief update model position and rotation
 *
 */
void	EnemyFrog::_updateModel() {
	if (_entityState.state != EntityState::RUNNING) {
		_model->transform.setPos(position + glm::vec3(movingSize.x / 2, 0.0, movingSize.z / 2));
	}

	// set model orientationdebug
	float	angle = glm::orientedAngle({0, 1}, glm::vec2(-front.x, front.z));
	_model->transform.setRot(angle);
}

/**
 * @brief update animation on state change
 *
 */
void	EnemyFrog::_updateAnimationState() {
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
				_model->animationSpeed = 1;
				_model->loopAnimation = false;
				_model->setAnimation("Armature|jump", &AEntity::animEndCb, this);
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
bool	EnemyFrog::init() {
	try {
		// if exist, delete last model
		if (_model)
			delete _model;

		OpenGLModel	&openglModel = ModelsManager::getModel("frog");
		_model = new Model(openglModel, game.getDtTime(), ETransform({1.5,
			0.0, 1.5}, ENEMY_FROG_SIZE));
		_model->play = true;
		_model->loopAnimation = true;

		// set speed according to animation duration
		_model->setAnimation("Armature|jump");
		speed = _model->getAnimDuration() / 1000 * FROG_JUMP_DIST;
		logDebug("speed: " << speed);

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
