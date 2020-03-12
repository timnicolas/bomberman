#include <unistd.h>
#include <stdlib.h>
#include <time.h>
// #include <bits/stdc++.h>

#include "SceneGame.hpp"
#include "bomberman.hpp"
#include "Player.hpp"
#include "Enemy.hpp"
#include "Wall.hpp"
#include "Crispy.hpp"
#include "Flag.hpp"
#include "End.hpp"

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
	{"empty", {EntityType::ENEMY, [](SceneGame &game) -> AEntity* {return Enemy::generateEnemy(game, 0.1f);}}},
};

// -- Constructors -------------------------------------------------------------

SceneGame::SceneGame(Gui * gui, float const &dtTime)
: AScene(gui, dtTime)
{
	player = nullptr;
	enemies = std::vector<ACharacter *>();
	flags = 0;
	size = {0, 0};
	level = 0;
	state = GameState::PLAY;
	time = std::chrono::milliseconds(0);
}

SceneGame::~SceneGame() {
	logInfo("Clean Game Level");

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
}

SceneGame::SceneGame(SceneGame const &src)
: AScene(src) {
	*this = src;
}

// -- Operators ----------------------------------------------------------------

SceneGame &SceneGame::operator=(SceneGame const &rhs) {
	if ( this != &rhs ) {
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
	logInfo("SceneGame init");
	_gui->enableCursor(false);
	_loadLevel(1);

	_gui->cam->pos = {size.x / 2, 25.0f, 2 * size.y};
	_gui->cam->lookAt(glm::vec3(
		size.x / 2, 1.0f,
		size.y / 1.61803398875f
	));

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
	if (pos.x < 0 || pos.x > size.x || pos.y < 0 || pos.y > size.y)
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
	logDebug("update");
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
	for (auto &&enemy : enemies) {
		if (!enemy->postUpdate())
			return false;
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
	logDebug("draw");
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

// -- Private Methods ----------------------------------------------------------

bool	SceneGame::_initJsonLevel(SettingsJson &lvl, uint8_t levelId) {
	logInfo("SceneGame _initJsonLevel");

	std::string		filename = "maps/level"+std::to_string(levelId)+".json";

	lvl.name("level"+std::to_string(levelId)).description("Level map");
	lvl.add<std::string>("level"+std::to_string(levelId)+"Filename", filename);

	// File json definition:
	lvl.add<std::string>("name");
	lvl.add<uint64_t>("height", 0).setMin(0);
	lvl.add<uint64_t>("width", 0).setMin(0);
	lvl.add<int64_t>("time", 0).setMin(-1).setMax(86400);

	lvl.add<SettingsJson>("objects");
		lvl.j("objects").add<std::string>("empty", " ");
		// unique player on game.
		lvl.j("objects").add<std::string>("player", "p");
		// destructing element dropped by the player.
		lvl.j("objects").add<std::string>("bomb", "x");
		// indestructible element outside the board
		lvl.j("objects").add<std::string>("wall", "w");
		// indestructible element of the board
		lvl.j("objects").add<std::string>("block", "b");
		// destructable element, who can give bonuses randomly
		lvl.j("objects").add<std::string>("crispy", "c");
		// flag to get end
		lvl.j("objects").add<std::string>("flag", "f");
		// end of level when all flag
		lvl.j("objects").add<std::string>("end", "e");
		// no spawn zone
		lvl.j("objects").add<std::string>("safe", "_");

	SettingsJson * mapPattern = new SettingsJson();
	mapPattern->add<std::string>("line", "");
	lvl.addList<SettingsJson>("map", mapPattern);

	SettingsJson * bonusPattern = new SettingsJson();
	bonusPattern->add<SettingsJson>("pos");
		bonusPattern->j("pos").add<uint64_t>("x", 0);
		bonusPattern->j("pos").add<uint64_t>("y", 0);
	bonusPattern->add<std::string>("type", "");
	lvl.addList<SettingsJson>("bonus", bonusPattern);

	try {
		if (lvl.loadFile(filename) == false) {
			// warning when loading settings
			return false;
		}
	} catch(SettingsJson::SettingsException const & e) {
		logErr(e.what());
		return false;
	}

	return true;
}

bool	SceneGame::_loadLevel(uint8_t levelId) {
	logInfo("SceneGame _loadLevel");
	SettingsJson	lvl;

	if (!_initJsonLevel(lvl, levelId))
		return false;

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
						player->init({i, 0, j});
						break;
					case EntityType::BOARD_FLAG:
						flags++;
					case EntityType::BOARD:
						board[i][j].push_back(entity);
						break;
					case EntityType::ENEMY:
						enemies.push_back(reinterpret_cast<ACharacter *>(entity));
						enemies.back()->init({i, 0, j});
						break;
					}
				}
			}
		}
	}

	return true;
}
