#include <unistd.h>
#include <stdlib.h>
#include <time.h>

#include "SceneGame.hpp"
#include "bomberman.hpp"
#include "FileUtils.hpp"
#include "Save.hpp"
#include "ModelsManager.hpp"
#include "BoxCollider.hpp"

#include "Player.hpp"
#include "Wall.hpp"
#include "Crispy.hpp"
#include "Flag.hpp"
#include "End.hpp"

#include "EnemyBasic.hpp"
#include "EnemyFollow.hpp"
#include "EnemyWithEye.hpp"
#include "EnemyFly.hpp"
#include "EnemyCrispy.hpp"
#include "EnemyFrog.hpp"

#include "AudioManager.hpp"
#include "SceneManager.hpp"

// -- Static members initialisation --------------------------------------------

std::map<std::string, SceneGame::Entity> SceneGame::entitiesCall = {
	{PLAYER_STR, {EntityType::PLAYER, [](SceneGame &game) -> AEntity* {return new Player(game);}}},
	{BOMB_STR, {EntityType::BOARD, [](SceneGame &game) -> AEntity* {return new Bomb(game);}}},
	{WALL_STR, {EntityType::BOARD, [](SceneGame &game) -> AEntity* {return new Wall(game);}}},
	{"block", {EntityType::BOARD, [](SceneGame &game) -> AEntity* {return new Wall(game, Block::BLOCK);}}},
	{CRISPY_STR, {EntityType::BOARD, [](SceneGame &game) -> AEntity* {return new Crispy(game);}}},
	{FLAG_STR, {EntityType::BOARD_FLAG, [](SceneGame &game) -> AEntity* {return new Flag(game);}}},
	{END_STR, {EntityType::BOARD, [](SceneGame &game) -> AEntity* {return new End(game);}}},
	{"safe", {EntityType::BOARD, [](SceneGame &game) -> AEntity* {(void)game; return nullptr;}}},
	{"empty", {EntityType::BOARD, [](SceneGame &game) -> AEntity* {(void)game; return nullptr;}}},
	{ENEMY_BASIC_STR, {EntityType::ENEMY, [](SceneGame &game) -> AEntity* {return new EnemyBasic(game);}}},
	{ENEMY_WITH_EYE_STR, {EntityType::ENEMY, [](SceneGame &game) -> AEntity* {return new EnemyWithEye(game);}}},
	{ENEMY_FOLLOW_STR, {EntityType::ENEMY, [](SceneGame &game) -> AEntity* {return new EnemyFollow(game);}}},
	{ENEMY_FLY_STR, {EntityType::ENEMY, [](SceneGame &game) -> AEntity* {return new EnemyFly(game);}}},
	{ENEMY_CRISPY_STR, {EntityType::ENEMY, [](SceneGame &game) -> AEntity* {return new EnemyCrispy(game);}}},
	{ENEMY_FROG_STR, {EntityType::ENEMY, [](SceneGame &game) -> AEntity* {return new EnemyFrog(game);}}},
};

// -- Constructors -------------------------------------------------------------

/**
 * @brief Construct a new Scene Game:: Scene Game object
 *
 * @param gui A pointer on the gui object
 * @param dtTime A reference to the delta time
 */
SceneGame::SceneGame(Gui * gui, float const &dtTime) : ASceneMenu(gui, dtTime) {
	player = nullptr;
	enemies = std::vector<AEnemy *>();
	flags = 0;
	size = {0, 0};
	level = NO_LEVEL;
	state = GameState::PLAY;
	levelTime = 0;
	time = 0;
	score.reset();
	levelEnemies = 0;
	levelCrispies = 0;
	_draw3dMenu = false;  // disable drawing 3D menu
	_terrain = nullptr;
	enemiesToKill = 0;
	enemiesKilled = 0;
	_alarm = false;
	_loadHelp = false;
	AudioManager::loadSound(INTROLEVEL_SOUND);
	AudioManager::loadSound(WIN_SOUND);
	AudioManager::loadSound(GAME_OVER_SOUND);
	AudioManager::loadSound(ALARM_TIME_SOUND);
}

/**
 * @brief Destroy the Scene Game:: Scene Game object
 */
SceneGame::~SceneGame() {
	_unloadLevel();  // delete all elements of current level if needed

	if (player != nullptr) {
		delete player;
	}

	for (auto it = _mapsList.begin(); it != _mapsList.end(); it++) {
		delete *it;
	}
	_mapsList.clear();

	delete _menuModels.player;
	delete _menuModels.robot;
	delete _menuModels.flower;
	delete _menuModels.fly;
	delete _menuModels.frog;
	delete _menuModels.crispy;
	delete _menuModels.follow;
	delete _terrain;
}

/**
 * @brief Construct a new Scene Game:: Scene Game object
 *
 * @param src The object to do the copy
 */
SceneGame::SceneGame(SceneGame const &src)
: ASceneMenu(src) {
	*this = src;
}

/**
 * @brief Construct a new Scene Game:: Draw For Menu:: Draw For Menu object
 */
SceneGame::DrawForMenu::DrawForMenu() {
	player = nullptr;
	flower = nullptr;
	robot = nullptr;
	fly = nullptr;
	frog = nullptr;
	crispy = nullptr;
	follow = nullptr;
}

/**
 * @brief Copy this object
 *
 * @param rhs The object to copy
 * @return SceneGame& A reference to the copied object
 */
SceneGame &SceneGame::operator=(SceneGame const &rhs) {
	if ( this != &rhs ) {
		logWarn("SceneGame object copied");
		board = rhs.board;
		boardFly = rhs.boardFly;
		player = rhs.player;
		enemies = rhs.enemies;
		flags = rhs.flags;
		size = rhs.size;
		level = rhs.level;
		state = rhs.state;
		levelTime = rhs.levelTime;
		time = rhs.time;
		score = rhs.score;
		levelEnemies = rhs.levelEnemies;
		levelCrispies = rhs.levelCrispies;
		enemiesToKill = rhs.enemiesToKill;
		enemiesKilled = rhs.enemiesKilled;
	}
	return *this;
}

/**
 * @brief Cout operator
 *
 * @param os The ostream object
 * @param myClass The class to cout
 * @return std::ostream& The ostream obj
 */
std::ostream &	operator<<(std::ostream & os, const SceneGame& myClass) {
	os << myClass.print();
	return (os);
}

/**
 * print params.
 */
std::string		SceneGame::print() const {
	std::string		str;
	str = "SceneGame info: [" + std::to_string(size.x) + ", "
	+ std::to_string(size.y) + "]";

	return str;
}

/**
 * @brief clear entity at position pos from board.
 *
 * @param entity
 * @param pos
 * @return true if cleared
 * @return false if not found
 */
bool			SceneGame::clearFromBoard(AEntity *entity, glm::vec2 pos) {
	std::vector<AEntity *> &box = board[pos.x][pos.y];
	std::vector<AEntity *>::iterator find = std::find(box.begin(), box.end(), entity);
	if (find == box.end())
		return false;
	box.erase(find);
	return true;
}

/**
 * @brief Check if the given pos is in the board.
 *
 * @param pos position
 * @param sz size
 * @return true if the position is in game
 */
bool	SceneGame::positionInGame(glm::vec3 pos, glm::vec3 sz) {
	if (pos.x < 0 || pos.x + sz.x > size.x || pos.z < 0 || pos.z + sz.z > size.y)
		return false;
	return true;
}

/**
 * @brief called when the scene is loaded
 */
void SceneGame::load() {
	_alarm = false;
	if (_gui->cam->getMode() == CamMode::FOLLOW_PATH) {
		state = GameState::INTRO;
		try {
			AudioManager::playSound(INTROLEVEL_SOUND, 1.0f, false, false, 2000);
		} catch(Sound::SoundException const & e) {
			logErr(e.what());
		}
	}
	else if (state == GameState::PAUSE
	|| state == GameState::WIN
	|| state == GameState::GAME_OVER) {
		state = GameState::PLAY;
	}
}
/**
 * @brief called when the scene is unloaded
 */
void SceneGame::unload() {
}

/**
 * @brief Insert an entity in the game
 *
 * @param name Entity name
 * @param pos Entity position
 * @param isFly If entity is flying
 * @param wallGenPercent Percentage of wall generation
 * @return false If failed
 */
bool SceneGame::insertEntity(std::string const & name, glm::ivec2 pos, bool isFly, uint64_t wallGenPercent) {
	AEntity * entity;

	/* if out of board */
	if (!positionInGame({pos.x, 0, pos.y})) {
		return false;
	}

	/* if invalid entity name */
	if (entitiesCall.find(name) == entitiesCall.end()) {
		logErr("invalid entity name " << name << " in SceneGame::insertEntity");
		return false;
	}

	/* if already an entity */
	if (isFly) {
		if (boardFly[pos.x][pos.y].size() > 0)
			return false;
	}
	else {
		if (board[pos.x][pos.y].size() > 0)
			return false;
	}

	// if it's empty, generate crispy wall with a certain probability
	if (name == "empty" && !isFly)
		entity = Crispy::generateCrispy(*this, wallGenPercent);
	else
		entity = entitiesCall[name].entity(*this);

	// do nothing on empty block
	if (entity == nullptr)
		return true;

	if (isFly) {
		if (entity->type == Type::WALL) {
			reinterpret_cast<AObject *>(entity)->isInFlyBoard = true;
			boardFly[pos.x][pos.y].push_back(entity);
		}
		else if (entitiesCall[name].entityType == EntityType::ENEMY) {
			enemies.push_back(reinterpret_cast<AEnemy *>(entity));
			enemies.back()->setPosition({pos.x, 1, pos.y});
		}
		else {
			logWarn("board fly can only contains walls and enemy");
		}
	}
	else {  // if not fly
		switch (entitiesCall[name].entityType) {
			case EntityType::PLAYER:
				if (player == nullptr) {
					player = reinterpret_cast<Player *>(entity);
					player->init();
					entity = nullptr;  // to avoid to call init a second time
				}
				else {
					delete entity;
					entity = nullptr;
				}
				player->setPosition({pos.x, 0, pos.y});
				break;
			case EntityType::BOARD_FLAG:
				flags++;
				board[pos.x][pos.y].push_back(entity);
				break;
			case EntityType::BOARD:
				if (entity->type == Type::BOMB) {
					if (player != nullptr) {
						reinterpret_cast<Bomb*>(entity)->setPropagation(player->bombProgation);
					}
					if (board[pos.x][pos.y].size() > 0) {
						delete entity;
						return false;
					}
				}
				board[pos.x][pos.y].push_back(entity);
				break;
			case EntityType::ENEMY:
				if (board[pos.x][pos.y].size() > 0) {
					logWarn("Cannot insert enemy on a block");
					delete entity;
					return false;
				}
				if (reinterpret_cast<AEnemy *>(entity)->getCollision({pos.x, 0, pos.y}).size()) {
					// don't create if we have a other Enemy at the same place
					delete entity;
					return false;
				}
				enemies.push_back(reinterpret_cast<AEnemy *>(entity));
				enemies.back()->setPosition({pos.x, 0, pos.y});
				break;
			default:
				delete entity;
		}
	}

	// init entity
	if (entity && !entity->init()) {
		return false;
	}

	return true;
}

// -- getter -------------------------------------------------------------------

/**
 * @brief Get the total number of levels
 *
 * @return uint32_t The number of levels
 */
uint32_t	SceneGame::getNbLevel() const { return _mapsList.size(); }
/**
 * @brief Get a level name
 *
 * @param levelId The level ID
 * @return std::string The level name
 */
std::string	SceneGame::getLevelName(int32_t levelId) const {
	if (levelId == NO_LEVEL)
		return "NO_LEVEL";
	if (static_cast<int32_t>(_mapsList.size()) <= levelId)
		throw SceneGameException(("Level " + std::to_string(levelId) + " do not exist.").c_str());
	return _mapsList[levelId]->s("name");
}
/**
 * @brief Get a level image
 *
 * @param levelId The level ID
 * @return std::string The level image path
 */
std::string	SceneGame::getLevelImg(int32_t levelId) const {
	if (levelId == NO_LEVEL) {
		logErr("can't get image for level 'NO_LEVEL'");
		return "";
	}
	return _mapsList[levelId]->s("img");
}

/**
 * @brief Return JSON Settings of level.
 *
 * @return SettingsJson& JSON Settings of level
 * @throw SceneGameException if error
 */
SettingsJson	&SceneGame::getSettingsLevel() const {
	if (level == NO_LEVEL)
		throw SceneGameException("no level set");
	if (level > (int32_t)_mapsList.size())
		throw SceneGameException(("unable to load level " + std::to_string(level)
		+ ": doesn't exist").c_str());
	return *(_mapsList[level]);
}

/**
 * @brief Get the name of all entity
 *
 * @return std::vector<std::string> The names
 */
std::vector<std::string> SceneGame::getAllEntityNames() {
	std::vector<std::string> res;
	for (auto && it : entitiesCall) {
		res.push_back(it.first);
	}
	return res;
}

// -- Exceptions errors --------------------------------------------------------

/**
 * @brief Construct a new Scene Game:: Scene Game Exception:: Scene Game Exception object
 */
SceneGame::SceneGameException::SceneGameException()
: std::runtime_error("SceneGame Exception") {}

/**
 * @brief Construct a new Scene Game:: Scene Game Exception:: Scene Game Exception object
 *
 * @param whatArg Error message
 */
SceneGame::SceneGameException::SceneGameException(const char* whatArg)
: std::runtime_error(std::string(std::string("SceneGameError: ") + whatArg).c_str()) {}
