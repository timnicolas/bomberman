#include "Player.hpp"
#include "Inputs.hpp"
#include <glm/gtx/vector_angle.hpp>

// -- Constructors -------------------------------------------------------------

Player::Player(SceneGame &game)
: ACharacter(game),
  _model(nullptr) {
	type = Type::PLAYER;
	name = "Player";
	resetParams();
}

Player::~Player() {
}

Player::Player(Player const &src) : ACharacter(src) {
	*this = src;
}

// -- Operators ----------------------------------------------------------------

Player &Player::operator=(Player const &rhs) {
	if ( this != &rhs ) {
		ACharacter::operator=(rhs);
		totalBombs = rhs.totalBombs;
		bombs = rhs.bombs;
		invulnerable = rhs.invulnerable;
		_toDraw = rhs._toDraw;
		passFire = rhs.passFire;
		passWall = rhs.passWall;
		detonator = rhs.detonator;
		passBomb = rhs.passBomb;
		bombProgation = rhs.bombProgation;
	}
	return *this;
}

// -- Methods ------------------------------------------------------------------

/**
 * @brief Init player for new levels
 *
 * @return true
 * @return false
 */
bool	Player::init() {
	invulnerable = 3.0f;
	bombs = totalBombs;

	try {
		OpenGLModel	&openglModel = ModelsManager::getModel("white");
		_model = new Model(openglModel, game.getDtTime(), ETransform({1, 0, 1}, {1.3, 1.3, 1.3}));
		_model->play = true;
		_model->loopAnimation = true;
		_model->setAnimation("Armature|idle", &AEntity::animEndCb, this);
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
 * @brief Set the Entity that the Character can cross
 */
void Player::resetCrossable() {
	ACharacter::resetCrossable();
}

/**
 * @brief Reset values for player.
 *
 */
void	Player::resetParams() {
	totalBombs = 1;
	bombs = totalBombs;
	speed = 3;
	alive = true;
	lives = 2;
	invulnerable = 3.0f;
	_toDraw = 0;
	bombProgation = 3;
	passFire = false;
	passWall = false;
	detonator = false;
	passBomb = false;
}

/**
 * @brief update is called each frame.
 *
 * @return true if success
 * @return false if failure
 */
bool	Player::update() {
	if (!active)
		return true;

	if (alive && _entityStatus.status != EntityStatus::DROP_BOMB) {
		// update invulnerability time
		if (invulnerable > 0.0f)
			invulnerable -= game.getDtTime();
		if (invulnerable < 0.0f)
			invulnerable = 0.0f;

		// move player
		_move();
		_model->transform.setPos(position + glm::vec3(.5, 0, .5));

		// set model orientation
		float	angle = glm::orientedAngle({0, 1}, glm::vec2(-front.x, front.z));
		_model->transform.setRot(angle);

		// drop bomb action
		if (Inputs::getKeyDown(InputType::ACTION)) {
			if (bombs > 0) {
				setStatus(EntityStatus::DROP_BOMB);
			}
		}
	}

	// update animation on status change
	if (_entityStatus.updated) {
		_entityStatus.updated = false;
		switch (_entityStatus.status) {
			case EntityStatus::IDLE:
				_model->animationSpeed = 1;
				_model->loopAnimation = true;
				_model->setAnimation("Armature|idle", &AEntity::animEndCb, this);
				break;
			case EntityStatus::DYING:
				_model->animationSpeed = 1;
				_model->loopAnimation = false;
				_model->setAnimation("Armature|death", &AEntity::animEndCb, this);
				break;
			case EntityStatus::RUNNING:
				_model->animationSpeed = 1;
				_model->loopAnimation = true;
				_model->setAnimation("Armature|run", &AEntity::animEndCb, this);
				break;
			case EntityStatus::DROP_BOMB:
				_model->animationSpeed = 10;
				_model->loopAnimation = false;
				_model->setAnimation("Armature|drop", &AEntity::animEndCb, this);
				break;
			case EntityStatus::VICTORY_EMOTE:
				_model->animationSpeed = 1;
				_model->loopAnimation = true;
				_model->setAnimation("Armature|dance", &AEntity::animEndCb, this);
				break;
			default:
				break;
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
bool	Player::draw(Gui &gui) {
	(void)gui;

	// blink if invulnerable
	if (invulnerable > 0) {
		_toDraw = ((_toDraw + 1) % 10);
		if (_toDraw > 5)
			return true;
	}

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
 * @brief Player Take <damage> damages.
 *
 * @param damage
 * @return true if damage taken
 * @return false if damage not taken
 */
bool	Player::takeDamage(const int damage) {
	bool wasAlive = alive;

	if (invulnerable <= 0.0f) {
		if (ACharacter::takeDamage(damage)) {
			if (alive) {
				invulnerable = 3.0f;
			}
			else if (wasAlive) {
				setStatus(EntityStatus::DYING);
			}
		}
	}
	return false;
}

/**
 * @brief Player take a <bonus> which allow to power up.
 *
 * @param bonus
 * @return true
 * @return false
 */
bool	Player::takeBonus(BonusType::Enum bonus) {
	switch (bonus) {
		case BonusType::LIFE:
			lives++;
			break;
		case BonusType::BOMBS:
			totalBombs++;
			bombs++;
			break;
		case BonusType::FLAMES:
			bombProgation++;
			break;
		case BonusType::SPEED:
			speed++;
			if (speed > MAX_SPEED)
				speed = MAX_SPEED;
			break;
		case BonusType::WALLPASS:
			if (std::find(crossableTypes.begin(), crossableTypes.end(), Type::CRISPY) == crossableTypes.end())
				crossableTypes.push_back(Type::CRISPY);
			passWall = true;
			break;
		case BonusType::DETONATOR:
			detonator = true;
			break;
		case BonusType::BOMBPASS:
			passBomb = true;
			if (std::find(crossableTypes.begin(), crossableTypes.end(), Type::BOMB) == crossableTypes.end())
				crossableTypes.push_back(Type::BOMB);
			break;
		case BonusType::FLAMPASS:
			passFire = true;
			break;
		case BonusType::SHIELD:
			invulnerable += 10.0f;
			break;
		case BonusType::TIME:
			game.time -= 15.0f;
			break;
		case BonusType::POINTS:
			game.score += 1500;
			break;
		default:
			break;
	}
	return true;
}

/**
 * @brief increment bomb, clamp to totalBombs
 *
 */
void	Player::addBomb() {
	bombs++;
	if (bombs > totalBombs)
		bombs = totalBombs;
}

/**
 * @brief called on animation end if passed to Model
 *
 * @param animName the current animation name
 */
void	Player::animEndCb(std::string animName) {
	// logDebug("animEndCb -> " << animName);
	if (animName == "Armature|drop") {
		_putBomb();
		setStatus(EntityStatus::IDLE);
	}
	else if (animName == "Armature|death") {
		logInfo("Player is dead.")
		game.state = GameState::GAME_OVER;
	}
}

// -- Private Methods ----------------------------------------------------------

void	Player::_move() {
	bool		moved = false;
	glm::vec3	dir = glm::vec3(0, front.y, 0);

	if (Inputs::getKey(InputType::UP)) {
		moved = true;
		dir.z -= 1;
	}
	if (Inputs::getKey(InputType::RIGHT)) {
		moved = true;
		dir.x += 1;
	}
	if (Inputs::getKey(InputType::DOWN)) {
		moved = true;
		dir.z += 1;
	}
	if (Inputs::getKey(InputType::LEFT)) {
		moved = true;
		dir.x -= 1;
	}
	_moveTo(dir, 1);

	// update status on end move
	if (!moved && _entityStatus.status == EntityStatus::RUNNING) {
		setStatus(EntityStatus::IDLE);
	}
}


void	Player::_putBomb() {
	if (bombs <= 0)
		return;

	glm::ivec2 intPos = getIntPos();
	if (game.board[intPos.x][intPos.y].size() == 0) {
		Bomb	*bomb = new Bomb(game);
		bomb->setPropagation(bombProgation);
		game.board[intPos.x][intPos.y].push_back(bomb);
		bombs -= 1;
	}
}

// -- Exceptions errors --------------------------------------------------------

Player::PlayerException::PlayerException()
: std::runtime_error("Player Exception") {}

Player::PlayerException::PlayerException(const char* whatArg)
: std::runtime_error(std::string(std::string("PlayerError: ") + whatArg).c_str()) {}
