#include <unistd.h>
#include <stdlib.h>
#include <time.h>

#include "SceneGame.hpp"
#include "bomberman.hpp"
#include "Player.hpp"
#include "Wall.hpp"
#include "Crispy.hpp"
#include "Flag.hpp"
#include "End.hpp"

#include "EnemyBasic.hpp"
#include "EnemyFollow.hpp"
#include "EnemyWithEye.hpp"

#include "SceneManager.hpp"

// -- Static members initialisation --------------------------------------------

std::map<std::string, SceneGame::Entity> SceneGame::_entitiesCall = {
	{"player", {EntityType::PLAYER, [](SceneGame &game) -> AEntity* {return new Player(game);}}},
	{"bomb", {EntityType::BOARD, [](SceneGame &game) -> AEntity* {return new Bomb(game);}}},
	{"wall", {EntityType::BOARD, [](SceneGame &game) -> AEntity* {return new Wall(game);}}},
	{"block", {EntityType::BOARD, [](SceneGame &game) -> AEntity* {return new Wall(game, Block::BLOCK);}}},
	{"crispy", {EntityType::BOARD, [](SceneGame &game) -> AEntity* {return new Crispy(game);}}},
	{"flag", {EntityType::BOARD_FLAG, [](SceneGame &game) -> AEntity* {return new Flag(game);}}},
	{"end", {EntityType::BOARD, [](SceneGame &game) -> AEntity* {return new End(game);}}},
	{"safe", {EntityType::BOARD, [](SceneGame &game) -> AEntity* {(void)game; return nullptr;}}},
	{"empty", {EntityType::ENEMY, [](SceneGame &game) -> AEntity* {return EnemyFollow::generateEnemy(game, 0.1f);}}},
};

// -- Constructors -------------------------------------------------------------

SceneGame::SceneGame(Gui * gui, float const &dtTime)
: AScene(gui, dtTime)
{
	player = nullptr;
	enemies = std::vector<AEnemy *>();
	flags = 0;
	size = {0, 0};
	level = NO_LEVEL;
	state = GameState::PLAY;
	time = std::chrono::milliseconds(0);
}

SceneGame::~SceneGame() {
	for (auto &&box : board) {
		for (auto &&row : box) {
			for (auto &&element : row) {
				delete element;
			}
		}
	}
	if (player != nullptr) {
		// TODO(ebaudet): save player if state is not GameOver.
		delete player;
	}
	auto enemy = enemies.begin();
	while (enemy != enemies.end()) {
		delete *enemy;
		// deleting an enemy also erase it from the list of enemies
		enemy = enemies.begin();
		if (enemy != enemies.end())
			enemy++;
	}

	for (auto it = _mapsList.begin(); it != _mapsList.end(); it++) {
		delete *it;
	}
	_mapsList.clear();
}

SceneGame::SceneGame(SceneGame const &src)
: AScene(src) {
	*this = src;
}

// -- Operators ----------------------------------------------------------------

SceneGame &SceneGame::operator=(SceneGame const &rhs) {
	if ( this != &rhs ) {
		logWarn("SceneGame object copied");
		board = rhs.board;
		player = rhs.player;
		enemies = rhs.enemies;
		flags = rhs.flags;
		size = rhs.size;
		level = rhs.level;
		state = rhs.state;
		time = rhs.time;
	}
	return *this;
}

std::ostream &	operator<<(std::ostream & os, const SceneGame& myClass) {
	os << myClass.print();
	return (os);
}

// -- Methods ------------------------------------------------------------------

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
 * init game method.
 */
bool			SceneGame::init() {
	_gui->enableCursor(false);
	int32_t i = 0;
	while (_initJsonLevel(i)) {
		if (i >= 100000) {  // max level
			break;
		}
		i++;
	}

	return true;
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
 * @param pos
 * @return true
 * @return false
 */
bool	SceneGame::positionInGame(glm::vec2 pos) {
	if (pos.x < 0 || pos.x > (size.x - 1) || pos.y < 0 || pos.y > (size.y - 1))
		return false;
	return true;
}

// -- AScene Methods -----------------------------------------------------------

/**
 * @brief update is called each frame.
 *
 * @return true
 * @return false
 */
bool	SceneGame::update() {
	if (level == NO_LEVEL)
		return true;

	if (Inputs::getKeyUp(InputType::CANCEL))
		state = GameState::PAUSE;

	// TODO(tnicolas42) remove the scene loader
	if (state == GameState::PAUSE) {
		SceneManager::loadScene(SceneNames::PAUSE);
		return true;
	}
	else if (state == GameState::WIN) {
		SceneManager::loadScene(SceneNames::VICTORY);
		return true;
	}
	else if (state == GameState::GAME_OVER) {
		// clear game infos.
		player->initParams();
		SceneManager::loadScene(SceneNames::GAME_OVER);
		return true;
	}

	for (auto &&board_it0 : board) {
		for (auto &&board_it1 : board_it0) {
			for (AEntity *board_it2 : board_it1) {
				if (!board_it2->update(_dtTime))
					return false;
			}
		}
	}
	for (auto &&enemy : enemies) {
		if (!enemy->update(_dtTime))
			return false;
	}
	player->update(_dtTime);

	return postUpdate();
}

/**
 * @brief postUpdate is called each frame, after update.
 *
 * @return true
 * @return false
 */
bool	SceneGame::postUpdate() {
	player->postUpdate();
	auto enemy = enemies.begin();
	while (enemy != enemies.end()) {
		if (!(*enemy)->postUpdate()) {
			enemy = enemies.begin();
		}
		if (enemy != enemies.end())
			enemy++;
	}

	for (auto &&board_it0 : board) {
		for (auto &&board_it1 : board_it0) {
			std::vector<AEntity *>::iterator it = board_it1.begin();
			AEntity * copy;
			while (it != board_it1.end()) {
				copy = *it;
				if (!(*it)->postUpdate())
					return false;
				if (it == board_it1.end())
					continue;
				if (copy == *it)
					it++;
			}
		}
	}

	return true;
}

/**
 * @brief draw is called each frame to draw the Game Scene.
 *
 * @return true
 * @return false
 */
bool	SceneGame::draw() {
	// use cubeShader, set uniform and activate textures
	glm::mat4	view = _gui->cam->getViewMatrix();
	_gui->cubeShader->use();
	_gui->cubeShader->setMat4("view", view);
	_gui->cubeShader->setVec3("viewPos", _gui->cam->pos);
	glBindVertexArray(_gui->cubeShVao);
	_gui->textureManager->activateTextures();
	_gui->cubeShader->setInt("blockId", 0);
	_gui->cubeShader->unuse();

	for (auto &&board_it0 : board) {
		for (auto &&board_it1 : board_it0) {
			for (AEntity *board_it2 : board_it1) {
				if (!board_it2->draw(*_gui))
					return false;
			}
		}
	}
	for (auto &&enemy : enemies) {
		if (!enemy->draw(*_gui))
			return false;
	}
	player->draw(*_gui);

	// draw board
	_gui->drawCube(Block::FLOOR, {0.0f, -0.3f, size.y - 1.0f}, {size.x, 0.3f, size.y});

	// release cubeShader and textures
	_gui->cubeShader->use();
	_gui->textureManager->disableTextures();
	_gui->cubeShader->unuse();

	// draw skybox
	_gui->drawSkybox(view);

	return true;
}

/**
 * @brief called when the scene is loaded
 */
void SceneGame::load() {
	if (state == GameState::PAUSE
	|| state == GameState::WIN
	|| state == GameState::GAME_OVER) {
		state = GameState::PLAY;
	}
	_gui->enableCursor(false);
}
/**
 * @brief called when the scene is unloaded
 */
void SceneGame::unload() {
}

/**
 * @brief load a level by ID
 *
 * @param levelId the level ID
 * @return true if the level loading is a success
 * @return false if the level loading failed
 */
bool SceneGame::loadLevel(int32_t levelId) {
	logInfo("load level " << levelId);
	if (_unloadLevel() == false) {
		level = NO_LEVEL;
		return false;
	}
	bool result = _loadLevel(levelId);

	_gui->cam->pos = {size.x / 2, 25.0f, 2 * size.y};
	_gui->cam->lookAt(glm::vec3(
		size.x / 2, 1.0f,
		size.y / 1.61803398875f
	));

	return result;
}

// -- Private Methods ----------------------------------------------------------

bool	SceneGame::_initJsonLevel(int32_t levelId) {
	// level$(levelID)
	std::string		levelName = "level" + std::to_string(levelId);
	// $(mapsPath)/$(levelName).json
	std::string		filename = s.s("mapsPath") + "/" + levelName + ".json";
	if (fileExists(filename) == false) {
		return false;  // file does not exist
	}

	SettingsJson *	lvl = new SettingsJson();

	lvl->name(levelName).description("Level map");
	lvl->add<std::string>(levelName + "Filename", filename);

	// File json definition:
	lvl->add<std::string>("name");
	lvl->add<std::string>("img", "bomberman-assets/img/icon_level1");
	lvl->add<uint64_t>("height", 0).setMin(0).setMax(100);
	lvl->add<uint64_t>("width", 0).setMin(0).setMax(100);
	lvl->add<int64_t>("time", 0).setMin(-1).setMax(86400);

	lvl->add<SettingsJson>("objects");
		lvl->j("objects").add<std::string>("empty", " ");
		// unique player on game.
		lvl->j("objects").add<std::string>("player", "p");
		// destructing element dropped by the player.
		lvl->j("objects").add<std::string>("bomb", "x");
		// indestructible element outside the board
		lvl->j("objects").add<std::string>("wall", "w");
		// indestructible element of the board
		lvl->j("objects").add<std::string>("block", "b");
		// destructable element, who can give bonuses randomly
		lvl->j("objects").add<std::string>("crispy", "c");
		// flag to get end
		lvl->j("objects").add<std::string>("flag", "f");
		// end of level when all flag
		lvl->j("objects").add<std::string>("end", "e");
		// no spawn zone
		lvl->j("objects").add<std::string>("safe", "_");

	SettingsJson * mapPattern = new SettingsJson();
	mapPattern->add<std::string>("line", "");
	lvl->addList<SettingsJson>("map", mapPattern);

	SettingsJson * bonusPattern = new SettingsJson();
	bonusPattern->add<SettingsJson>("pos");
		bonusPattern->j("pos").add<uint64_t>("x", 0);
		bonusPattern->j("pos").add<uint64_t>("y", 0);
	bonusPattern->add<std::string>("type", "");
	lvl->addList<SettingsJson>("bonus", bonusPattern);

	try {
		if (lvl->loadFile(filename) == false) {
			// warning when loading settings
			return true;
		}
	} catch(SettingsJson::SettingsException const & e) {
		logErr(e.what());
		return false;
	}

	_mapsList.push_back(lvl);

	return true;
}

bool	SceneGame::_unloadLevel() {
	if (level == NO_LEVEL)
		return true;

	for (auto &&box : board) {
		for (auto &&row : box) {
			for (auto &&element : row) {
				delete element;
			}
		}
	}
	board.clear();
	auto enemy = enemies.begin();
	while (enemy != enemies.end()) {
		delete *enemy;
		// deleting an enemy also erase it from the list of enemies
		enemy = enemies.begin();
		if (enemy != enemies.end())
			enemy++;
	}
	enemies.clear();
	level = NO_LEVEL;
	return true;
}

/**
 * @brief Load Level method. Throw a SceneException if the level is incomplete.
 *
 * @param levelId
 * @return true
 * @return false
 */
bool	SceneGame::_loadLevel(int32_t levelId) {
	if (levelId == NO_LEVEL)
		return true;
	if (levelId > (int32_t)_mapsList.size()) {
		logErr("unable to load level " << levelId << ": doesn't exist");
		return false;
	}

	level = levelId;  // save new level ID
	SettingsJson & lvl = *(_mapsList[level]);

	// Getting json info
	size = {lvl.u("width"), lvl.u("height")};
	board = std::vector< std::vector<std::vector<AEntity*>> >(size.x,
			std::vector< std::vector<AEntity*> >(size.y,
			std::vector< AEntity* >()));

	if (lvl.lj("map").list.size() != size.y)
		throw SceneException("Map height error");

	time = std::chrono::seconds(lvl.i("time"));

	flags = 0;
	AEntity *entity;
	for (uint32_t j = 0; j < size.y; j++) {
		std::string line = lvl.lj("map").list[j]->s("line");
		if (line.length() != size.x)
			throw SceneException(("Map width error on line "+std::to_string(j)).c_str());
		for (uint32_t i = 0; i < size.x; i++) {
			for (auto &&entitYCall : _entitiesCall) {
				if (line[i] == lvl.j("objects").s(entitYCall.first)[0]) {
					entity = _entitiesCall[entitYCall.first].entity(*this);
					if (entity == nullptr)
						continue;
					switch (_entitiesCall[entitYCall.first].entityType) {
					case EntityType::PLAYER:
						if (player == nullptr)
							player = reinterpret_cast<Player *>(entity);
						player->setPosition({i, 0, j});
						break;
					case EntityType::BOARD_FLAG:
						flags++;
						board[i][j].push_back(entity);
						break;
					case EntityType::BOARD:
						board[i][j].push_back(entity);
						break;
					case EntityType::ENEMY:
						enemies.push_back(reinterpret_cast<AEnemy *>(entity));
						enemies.back()->setPosition({i, 0, j});
						break;
					}
				}
			}
		}
	}

	if (player == nullptr)
		throw SceneException("No player on this level.");

	bool	end = false;
	for (uint32_t j = 0; j < size.y; j++) {
		for (uint32_t i = 0; i < size.x; i++) {
			for(auto entity : board[i][j]) {
				if (entity->type == Type::END)
					end = true;
			}
		}
	}
	if (!end)
		throw SceneException("No end on this level.");

	// set camera
	_gui->cam->lookAt(glm::vec3(size.x / 2 + 0.5f, 1.0f, size.y * 0.7f));

	return true;
}

// -- getter -------------------------------------------------------------------

uint32_t	SceneGame::getNbLevel() const { return _mapsList.size(); }
std::string	SceneGame::getLevelName(int32_t levelId) const {
	if (levelId == NO_LEVEL)
		return "NO_LEVEL";
	return _mapsList[levelId]->s("name");
}
std::string	SceneGame::getLevelImg(int32_t levelId) const {
	if (levelId == NO_LEVEL) {
		logErr("can't get image for level 'NO_LEVEL'");
		return "";
	}
	return _mapsList[levelId]->s("img");
}
