#include <unistd.h>
#include <stdlib.h>
#include <time.h>
// #include <bits/stdc++.h>

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

SceneGame::SceneGame(Gui * gui)
: AScene(gui)
{
	player = nullptr;
	enemies = std::vector<ACharacter *>();
	bombs = std::vector<Bomb *>();
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
: AScene(src)
{
	*this = src;
}

// -- Operators ----------------------------------------------------------------

SceneGame &SceneGame::operator=(SceneGame const &rhs) {
	if ( this != &rhs ) {
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

std::ostream &	operator<<(std::ostream & os, const SceneGame& my_class) {
	os << my_class.print();
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
	return true;
}

// -- Private Methods ----------------------------------------------------------

bool	SceneGame::update(std::chrono::milliseconds last_loop_ms) {
	(void)last_loop_ms;
	// for (auto &&board_it1 : board) {
	// 	for (auto &&board_it1 : board_it1) {
	// 		for (AEntity *board_it2 : board_it1) {
	// 			if (!board_it2->update(getMs() - last_loop_ms))
	// 				return false;
	// 		}
	// 	}
	// }
	// for (auto &&enemy : enemies) {
	// 	if (!enemy->update(getMs() - last_loop_ms))
	// 		return false;
	// }
	// for (auto &&bomb : bombs) {
	// 	if (!bomb->update(getMs() - last_loop_ms))
	// 		return false;
	// }
	// player->update(getMs() - last_loop_ms);

	return true;
}

bool	SceneGame::draw() {
	// for (auto &&board_it1 : board) {
	// 	for (auto &&board_it1 : board_it1) {
	// 		for (AEntity *board_it2 : board_it1) {
	// 			if (!board_it2->draw())
	// 				return false;
	// 		}
	// 	}
	// }
	// for (auto &&enemy : enemies) {
	// 	if (!enemy->draw())
	// 		return false;
	// }
	// for (auto &&bomb : bombs) {
	// 	if (!bomb->draw())
	// 		return false;
	// }
	// player->draw();

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

bool	SceneGame::_initJsonLevel(SettingsJson &lvl, uint8_t level_id) {
	logInfo("SceneGame _initJsonLevel");

	std::string		filename = "maps/level"+std::to_string(level_id)+".json";

	lvl.name("level"+std::to_string(level_id)).description("Level map");
	lvl.add<std::string>("level"+std::to_string(level_id)+"Filename", filename);

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

bool	SceneGame::_loadLevel(uint8_t level_id) {
	logInfo("SceneGame _loadLevel");
	SettingsJson	lvl;

	if (!_initJsonLevel(lvl, level_id))
		return false;

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
					switch (_entitiesCall[entitYCall.first].entity_type) {
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
	_gui->cubeShader->setVec3("blockSize",
		{_gui->gameInfo.gameboard.x, 1.0f, _gui->gameInfo.gameboard.y});
	_gui->cubeShader->setInt("blockId", Block::FLOOR);  // set block type
	pos = glm::vec3(0.0f, -1.0f, _gui->gameInfo.gameboard.y - 1.0f);
	model = glm::translate(glm::mat4(1.0), pos);
	_gui->cubeShader->setMat4("model", model);
	glDrawArrays(GL_POINTS, 0, C_NB_FACES);  // draw

	// - draw wall -------------------------------------------------------------
	_gui->cubeShader->setInt("blockId", Block::DURABLE_WALL);  // set block type
	// board side 0
	_gui->cubeShader->setVec3("blockSize",
		{_gui->gameInfo.gameboard.x + 1.0f, 2.0f, 1.0f});
	pos = glm::vec3(0.0f, -1.0f, _gui->gameInfo.gameboard.y);
	model = glm::translate(glm::mat4(1.0), pos);
	_gui->cubeShader->setMat4("model", model);
	glDrawArrays(GL_POINTS, 0, C_NB_FACES);  // draw

	// board side 1
	_gui->cubeShader->setVec3("blockSize",
		{1.0f, 2.0f, _gui->gameInfo.gameboard.y + 1.0f});
	pos = glm::vec3(_gui->gameInfo.gameboard.x, -1.0f, _gui->gameInfo.gameboard.y - 1.0f);
	model = glm::translate(glm::mat4(1.0), pos);
	_gui->cubeShader->setMat4("model", model);
	glDrawArrays(GL_POINTS, 0, C_NB_FACES);  // draw

	// board side 2
	_gui->cubeShader->setVec3("blockSize",
		{_gui->gameInfo.gameboard.x + 1.0f, 2.0f, 1.0f});
	pos = glm::vec3(-1.0f, -1.0f, -1.0f);
	model = glm::translate(glm::mat4(1.0), pos);
	_gui->cubeShader->setMat4("model", model);
	glDrawArrays(GL_POINTS, 0, C_NB_FACES);  // draw

	// board side 3
	_gui->cubeShader->setVec3("blockSize",
		{1.0f, 2.0f, _gui->gameInfo.gameboard.y + 1.0f});
	pos = glm::vec3(-1.0f, -1.0f, _gui->gameInfo.gameboard.y);
	model = glm::translate(glm::mat4(1.0), pos);
	_gui->cubeShader->setMat4("model", model);
	glDrawArrays(GL_POINTS, 0, C_NB_FACES);  // draw


	// -- draw player ----------------------------------------------------------
	_gui->cubeShader->setVec3("blockSize", {1.0f, 1.0f, 1.0f});
	if (_gui->gameInfo.player != VOID_POS) {
		// set block type
		_gui->cubeShader->setInt("blockId", Block::PLAYER);
		// set block pos
		pos = glm::vec3(_gui->gameInfo.player[0], 0.0f, _gui->gameInfo.player[1]);
		model = glm::translate(glm::mat4(1.0), pos);
		_gui->cubeShader->setMat4("model", model);
		glDrawArrays(GL_POINTS, 0, C_NB_FACES);  // draw
	}

	// -- draw bomb ------------------------------------------------------------
	_gui->cubeShader->setVec3("blockSize", {1.0f, 1.0f, 1.0f});
	// set block type
	_gui->cubeShader->setInt("blockId", Block::BOMB);
	// set block pos
	pos = glm::vec3(10, 0.0f, 5);
	model = glm::translate(glm::mat4(1.0), pos);
	_gui->cubeShader->setMat4("model", model);
	glDrawArrays(GL_POINTS, 0, C_NB_FACES);  // draw

	// -- draw durable wall ----------------------------------------------------
	_gui->cubeShader->setVec3("blockSize", {1.0f, 1.0f, 1.0f});
	// set block type
	_gui->cubeShader->setInt("blockId", Block::DURABLE_WALL);
	// set block pos
	pos = glm::vec3(7, 0.0f, 7);
	model = glm::translate(glm::mat4(1.0), pos);
	_gui->cubeShader->setMat4("model", model);
	glDrawArrays(GL_POINTS, 0, C_NB_FACES);  // draw

	// -- draw durable wall ----------------------------------------------------
	_gui->cubeShader->setVec3("blockSize", {1.0f, 1.0f, 1.0f});
	// set block type
	_gui->cubeShader->setInt("blockId", Block::DESTRUCTIBLE_WALL);
	// set block pos
	pos = glm::vec3(7, 0.0f, 12);
	model = glm::translate(glm::mat4(1.0), pos);
	_gui->cubeShader->setMat4("model", model);
	glDrawArrays(GL_POINTS, 0, C_NB_FACES);  // draw
}
