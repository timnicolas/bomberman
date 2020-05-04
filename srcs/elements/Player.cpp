#include "Player.hpp"
#include "Inputs.hpp"
#include "Save.hpp"
#include "AudioManager.hpp"
#include "SceneCheatCode.hpp"

// -- Constructors -------------------------------------------------------------

Player::Player(SceneGame &game)
: ACharacter(game) {
	_model = nullptr;
	size = glm::vec3(0.7, 1.5, 0.7);
	movingSize = size;
	type = Type::PLAYER;
	name = PLAYER_STR;
	resetParams();
	AudioManager::loadSound(PLAYER_HURT_SOUND);
	AudioManager::loadSound(PLAYER_DEATH_SOUND);
	AudioManager::loadSound(PLAYER_RUN_SOUND);
	AudioManager::loadSound(PLAYER_ONE_LIFE_SOUND);
	AudioManager::loadSound(PUT_BOMB_SOUND);
	AudioManager::loadSound(PUT_BOMB_EMPTY_SOUND);
}

Player::~Player() {
	if (_entityState.state == EntityState::RUNNING) {
		AudioManager::stopSound(PLAYER_RUN_SOUND);
	}
}

Player::Player(Player const &src) : Player(src.game) {
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
		// if exist, delete last model
		glm::vec3 tmpPos = {1, 0, 1};
		if (_model) {
			tmpPos = _model->transform.getPos();
			delete _model;
		}

		OpenGLModel	&openglModel = ModelsManager::getModel("white");
		_model = new Model(openglModel, game.getDtTime(), ETransform(tmpPos, PLAYER_SIZE));
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
	if (lives > static_cast<int>(Save::getDifficulty()))
		lives = Save::getDifficulty();
	invulnerable = 3.0f;
	_toDraw = 0;
	bombProgation = 2;
	passFire = false;
	passWall = false;
	detonator = false;
	passBomb = false;
	resetCrossable();
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

	if (alive && _entityState.state != EntityState::DROP_BOMB) {
		// update invulnerability time
		if (invulnerable > 0.0f)
			invulnerable -= game.getDtTime();
		if (invulnerable < 0.0f)
			invulnerable = 0.0f;

		// move player
		_move();
		_model->transform.setPos(position + glm::vec3(movingSize.x / 2, 0, movingSize.z / 2));

		// set model orientation
		float	angle = glm::orientedAngle({0, 1}, glm::vec2(-front.x, front.z));
		_model->transform.setRot(angle);

		// drop bomb action
		if (Inputs::getKeyDown(InputType::ACTION)) {
			if (bombs > 0) {
				setState(EntityState::DROP_BOMB);
				try {
					AudioManager::stopSound(PUT_BOMB_SOUND);
					AudioManager::playSound(PUT_BOMB_SOUND);
				} catch(Sound::SoundException const & e) {
					logErr(e.what());
				}
			} else {
				try {
					AudioManager::playSound(PUT_BOMB_EMPTY_SOUND);
				} catch(Sound::SoundException const & e) {
					logErr(e.what());
				}
			}
		}
	}

	// update animation on state change
	_updateAnimationState();

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
	SceneGame & scGame = *reinterpret_cast<SceneGame *>(SceneManager::getScene(SceneNames::GAME));

	// blink if invulnerable
	if (scGame.state == GameState::PLAY && invulnerable > 0) {
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
	bool was_alive = alive;
	if (invulnerable <= 0.0f) {
		if (ACharacter::takeDamage(damage)) {
			if (alive) {
				try {
					AudioManager::playSound(PLAYER_HURT_SOUND);
				} catch(Sound::SoundException const & e) {
					logErr(e.what());
				}
				if (lives == 1) {
					try {
						AudioManager::playSound(PLAYER_ONE_LIFE_SOUND);
					} catch(Sound::SoundException const & e) {
						logErr(e.what());
					}
				}
				invulnerable = 3.0f;
			} else if (was_alive) {
				try {
					AudioManager::playSound(PLAYER_DEATH_SOUND);
				} catch(Sound::SoundException const & e) {
					logErr(e.what());
				}
			}
		}
		return true;
	}
	return false;
}

/**
 * @brief Player take a <bonus> which allow to power up.
 *
 * @param bonus
 * @param silent
 * @return true
 * @return false
 */
bool	Player::takeBonus(BonusType::Enum bonus, bool silent) {
	switch (bonus) {
		case BonusType::LIFE:
			lives++;
			if (lives > static_cast<int>(Save::getDifficulty()))
				lives = Save::getDifficulty();
			break;
		case BonusType::BOMBS:
			totalBombs++;
			bombs++;
			if (totalBombs > 10)
				totalBombs = 10;
			if (bombs > 10)
				bombs = 10;
			break;
		case BonusType::FLAMES:
			bombProgation++;
			if (bombProgation > 12)
				bombProgation = 12;
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
	if (!silent) {
		SceneCheatCode & scCheatCode = *reinterpret_cast<SceneCheatCode *>(
			SceneManager::getScene(SceneNames::CHEAT_CODE)
		);
		scCheatCode.clearAllLn();
		logInfoScreen(Bonus::getDescription(bonus));
	}
	return true;
}

/**
 * @brief Player remove a <bonus>
 *
 * @param bonus
 * @return true
 * @return false
 */
bool	Player::rmBonus(BonusType::Enum bonus) {
	float tmp;
	switch (bonus) {
		case BonusType::LIFE:
			tmp = invulnerable;
			invulnerable = 0;
			takeDamage(1);
			if (!alive)
				_updateAnimationState();
			else if (tmp > invulnerable)
				invulnerable = tmp;
			break;
		case BonusType::BOMBS:
			if (totalBombs > 1)
				totalBombs--;
			if (bombs > totalBombs)
				bombs--;
			break;
		case BonusType::FLAMES:
			if (bombProgation > 1)
				bombProgation--;
			break;
		case BonusType::SPEED:
			speed--;
			if (speed < MIN_SPEED)
				speed = MIN_SPEED;
			break;
		case BonusType::WALLPASS:
			while (1) {
				auto it = std::find(crossableTypes.begin(), crossableTypes.end(), Type::CRISPY);
				if (it == crossableTypes.end())
					break;
				else
					crossableTypes.erase(it);
			}
			passWall = false;
			break;
		case BonusType::DETONATOR:
			detonator = false;
			break;
		case BonusType::BOMBPASS:
			passBomb = false;
			while (1) {
				auto it = std::find(crossableTypes.begin(), crossableTypes.end(), Type::BOMB);
				if (it == crossableTypes.end())
					break;
				else
					crossableTypes.erase(it);
			}
			break;
		case BonusType::FLAMPASS:
			passFire = false;
			break;
		case BonusType::SHIELD:
			invulnerable = 0;
			break;
		case BonusType::TIME:
			game.time += 15.0f;
			break;
		case BonusType::POINTS:
			game.score -= 1500;
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
 * @brief update animation on state change
 *
 */
void	Player::_updateAnimationState() {
	if (_entityState.updated) {
		_entityState.updated = false;
		switch (_entityState.state) {
			case EntityState::IDLE:
				AudioManager::stopSound(PLAYER_RUN_SOUND);
				_model->animationSpeed = 1;
				_model->loopAnimation = true;
				_model->setAnimation("Armature|idle", &AEntity::animEndCb, this);
				break;
			case EntityState::DYING:
				AudioManager::stopSound(PLAYER_RUN_SOUND);
				_model->animationSpeed = 1;
				_model->loopAnimation = false;
				_model->setAnimation("Armature|death", &AEntity::animEndCb, this);
				break;
			case EntityState::RUNNING:
				try {
					AudioManager::playSound(PLAYER_RUN_SOUND, 1.0, true);
				} catch(Sound::SoundException const & e) {
					logErr(e.what());
				}
				_model->animationSpeed = 1;
				_model->loopAnimation = true;
				_model->setAnimation("Armature|run", &AEntity::animEndCb, this);
				break;
			case EntityState::DROP_BOMB:
				AudioManager::stopSound(PLAYER_RUN_SOUND);
				_model->animationSpeed = 10;
				_model->loopAnimation = false;
				_model->setAnimation("Armature|drop", &AEntity::animEndCb, this);
				break;
			case EntityState::VICTORY_EMOTE:
				AudioManager::stopSound(PLAYER_RUN_SOUND);
				_model->animationSpeed = 1;
				_model->loopAnimation = true;
				_model->setAnimation("Armature|dance", &AEntity::animEndCb, this);
				break;
			default:
				break;
		}
	}
}

/**
 * @brief called on animation end if passed to Model
 *
 * @param animName the current animation name
 */
void	Player::animEndCb(std::string animName) {
	if (animName == "Armature|drop") {
		_putBomb();
		setState(EntityState::IDLE);
	}
	else if (animName == "Armature|death") {
		_animDeathEnd = true;
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

	if (moved) {
		_moveTo(dir);
	}
	// update state on end move
	else if (_entityState.state == EntityState::RUNNING) {
		setState(EntityState::IDLE);
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
		bomb->init();
		bombs -= 1;
	}
}

// -- Exceptions errors --------------------------------------------------------

Player::PlayerException::PlayerException()
: std::runtime_error("Player Exception") {}

Player::PlayerException::PlayerException(const char* whatArg)
: std::runtime_error(std::string(std::string("PlayerError: ") + whatArg).c_str()) {}
