#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <filesystem>

#include "SceneGame.hpp"
#include "bomberman.hpp"
#include "Player.hpp"
#include "Enemy.hpp"
#include "Wall.hpp"
#include "Flag.hpp"
#include "End.hpp"

// -- Static members initialisation --------------------------------------------

std::map<std::string, SceneGame::Entity> SceneGame::_entitiesCall = {
	{"player", {EntityType::PLAYER, [](SceneGame &game) -> AEntity* {return new Player(game);}}},
	{"bomb", {EntityType::BOMB, [](SceneGame &game) -> AEntity* {return new Bomb(game);}}},
	{"wall", {EntityType::BOARD, [](SceneGame &game) -> AEntity* {return new Wall(game);}}},
	{"block", {EntityType::BOARD, [](SceneGame &game) -> AEntity* {return new Wall(game);}}},
	{"crispy", {EntityType::BOARD, [](SceneGame &game) -> AEntity* {return new Wall(game);}}},
	{"flag", {EntityType::BOARD_FLAG, [](SceneGame &game) -> AEntity* {return new Flag(game);}}},
	{"end", {EntityType::BOARD, [](SceneGame &game) -> AEntity* {return new End(game);}}},
	{"safe", {EntityType::BOARD, [](SceneGame &game) -> AEntity* {(void)game; return nullptr;}}},
	{"empty", {EntityType::ENEMY, [](SceneGame &game) -> AEntity* {return Enemy::generateEnemy(game, 1.0);}}},
};

// -- Constructors -------------------------------------------------------------

SceneGame::SceneGame(Gui * gui, float const &dtTime)
: AScene(gui, dtTime)
{
	player = nullptr;
	enemies = std::vector<ACharacter *>();
	bombs = std::vector<Bomb *>();
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
	for (auto &&enemy : enemies) {
		delete enemy;
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
		bombs = rhs.bombs;
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

bool	SceneGame::update() {
	// for (auto &&board_it1 : board) {
	// 	for (auto &&board_it1 : board_it1) {
	// 		for (AEntity *board_it2 : board_it1) {
	// 			if (!board_it2->update(_dtTime))
	// 				return false;
	// 		}
	// 	}
	// }
	// for (auto &&enemy : enemies) {
	// 	if (!enemy->update(_dtTime))
	// 		return false;
	// }
	// for (auto &&bomb : bombs) {
	// 	if (!bomb->update(_dtTime))
	// 		return false;
	// }
	// player->update(_dtTime);

	return true;
}

bool	SceneGame::draw() {
	// use cubeShader, set uniform and activate textures
	glm::mat4	view = _gui->cam->getViewMatrix();
	_gui->cubeShader->use();
	_gui->cubeShader->setMat4("view", view);
	_gui->cubeShader->setVec3("viewPos", _gui->cam->pos);
	glBindVertexArray(_gui->cubeShVao);
	_gui->textureManager->activateTextures();
	_gui->cubeShader->setInt("blockId", 0);

	// draw scene
	_drawBoard();

	// release cubeShader and textures
	_gui->textureManager->disableTextures();
	_gui->cubeShader->unuse();

	// draw skybox
	_gui->drawSkybox(view);

	return true;
}

/**
 * @brief called when the scene is loaded
 *
 */
void SceneGame::load() {
	_gui->enableCursor(false);
}
/**
 * @brief called when the scene is unloaded
 *
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
	return _loadLevel(levelId);
}

// -- Private Methods ----------------------------------------------------------

bool	SceneGame::_initJsonLevel(int32_t levelId) {
	// level$(levelID)
	std::string		levelName = "level" + std::to_string(levelId);
	// $(mapsPath)/$(levelName).json
	std::string		filename = s.s("mapsPath") + "/" + levelName + ".json";
	if (std::__fs::filesystem::exists(filename) == false) {
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
	for (auto &&enemy : enemies) {
		delete enemy;
	}
	level = NO_LEVEL;
	return true;
}

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
						player->init({i, j});
						break;
					case EntityType::BOMB:
						bombs.push_back(reinterpret_cast<Bomb *>(entity));
						board[i][j].push_back(entity);
						break;
					case EntityType::BOARD_FLAG:
						flags++;
					case EntityType::BOARD:
						board[i][j].push_back(entity);
						break;
					case EntityType::ENEMY:
						enemies.push_back(reinterpret_cast<ACharacter *>(entity));
						enemies.back()->init({i, j});
						break;
					}
				}
			}
		}
	}

	return true;
}

// -- _drawBoard ---------------------------------------------------------------
void	SceneGame::_drawBoard() {
	glm::mat4 model(1.0);
	glm::vec3 pos;

	// board floor
	_gui->cubeShader->setVec3("blockSize", {size.x, 1.0f, size.y});
	_gui->cubeShader->setInt("blockId", Block::FLOOR);  // set block type
	pos = glm::vec3(0.0f, -1.0f, size.y - 1);
	model = glm::translate(glm::mat4(1.0), pos);
	_gui->cubeShader->setMat4("model", model);
	glDrawArrays(GL_POINTS, 0, C_NB_FACES);  // draw

	// -- draw entity ----------------------------------------------------------
	_gui->cubeShader->setVec3("blockSize", {1.0f, 1.0f, 1.0f});
	for (uint32_t x = 0; x < size.x; x++) {
		for (uint32_t y = 0; y < size.y; y++) {
			for (AEntity *entity : board[x][y]) {
				bool draw = true;
				switch (entity->type) {
					case Type::WALL:
						_gui->cubeShader->setInt("blockId", Block::DURABLE_WALL);
						break;
					default:
						draw = false;
						break;
				}
				if (draw) {
					// set block pos
					pos = glm::vec3(x, 0.0f, y);
					model = glm::translate(glm::mat4(1.0), pos);
					_gui->cubeShader->setMat4("model", model);
					glDrawArrays(GL_POINTS, 0, C_NB_FACES);  // draw
				}
			}
		}
	}
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
