#include <unistd.h>
#include <stdlib.h>
#include <time.h>
// #include <bits/stdc++.h>

#include "SceneGame.hpp"
#include "bomberman.hpp"

// -- Static members initialisation --------------------------------------------

std::map<std::string, AEntity *> SceneGame::_entitiesCall = {
	{"player", new Player()},
	{"bomb", new Bomb()},
};

// -- Constructors -------------------------------------------------------------

SceneGame::SceneGame()
: AScene()
{
	// TODO(ebaudet): init members
}

SceneGame::~SceneGame() {
}

SceneGame::SceneGame(SceneGame const &src)
: AScene()
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
	// _loadLevel(1);
	return true;
}

// -- Private Methods ----------------------------------------------------------

bool	SceneGame::update(std::chrono::milliseconds last_loop_ms) {
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

bool	SceneGame::draw(Gui * gui) {
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
	glm::mat4	view = gui->cam->getViewMatrix();
	gui->cubeShader->use();
	gui->cubeShader->setMat4("view", view);
	gui->cubeShader->setVec3("viewPos", gui->cam->pos);
	glBindVertexArray(gui->cubeShVao);
	gui->textureManager->activateTextures();
	gui->cubeShader->setInt("blockId", 0);

	// draw scene
	_drawBoard(gui);

	// release cubeShader and textures
	gui->textureManager->disableTextures();
	gui->cubeShader->unuse();

	// draw skybox
	gui->drawSkybox(view);

	return true;
}

bool	SceneGame::_loadLevel(uint8_t level) {
	SettingsJson	lvl;
	std::string		filename = "bomberman-assets/maps/level"+std::to_string(level)+".json";

	// File json definition:

	lvl.name("level"+std::to_string(level)).description("Level map");
	lvl.add<std::string>("level"+std::to_string(level)+"Filename", filename);
	lvl.add<std::string>("name");
	lvl.add<uint64_t>("height", 0).setMin(0);
	lvl.add<uint64_t>("width", 0).setMin(0);

	lvl.add<SettingsJson>("objects");
		lvl.j("objects").add<std::string>("wall", "w");
		lvl.j("objects").add<std::string>("empty", " ");
		lvl.j("objects").add<std::string>("player", "p");
		lvl.j("objects").add<std::string>("tree", "t");
		lvl.j("objects").add<std::string>("box", "b");
		lvl.j("objects").add<std::string>("end", "e");
		lvl.j("objects").add<std::string>("bomb", "x");

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

	// Getting json info

	size = {lvl.u("width"), lvl.u("height")};
	board = std::vector< std::vector<std::vector<AEntity*>> >(size.x,
			std::vector< std::vector<AEntity*> >(size.y,
			std::vector< AEntity* >()));

	if (lvl.lj("map").list.size() != size.y)
		throw GameException("Map height error");
	for (uint32_t j = 0; j < size.y; j++) {
		std::string line = lvl.lj("map").list[j]->s("line");
		if (line.length() != size.x)
			throw GameException(("Map width error on line "+std::to_string(j)).c_str());
		for (uint32_t i = 0; i < size.x; i++) {
			for (auto &&entitYCall : _entitiesCall) {
				if (line[i] == lvl.j("objects").s(entitYCall.first)[0]) {
					board[i][j].push_back(_entitiesCall[entitYCall.first]);
				}
			}
		}
	}

	return true;
}

// -- _drawBoard ---------------------------------------------------------------
void	SceneGame::_drawBoard(Gui * gui) {
	glm::mat4 model(1.0);
	glm::vec3 pos;

	// board floor
	gui->cubeShader->setVec3("blockSize",
		{gui->gameInfo.gameboard.x, 1.0f, gui->gameInfo.gameboard.y});
	gui->cubeShader->setInt("blockId", Block::FLOOR);  // set block type
	pos = glm::vec3(0.0f, -1.0f, gui->gameInfo.gameboard.y - 1.0f);
	model = glm::translate(glm::mat4(1.0), pos);
	gui->cubeShader->setMat4("model", model);
	glDrawArrays(GL_POINTS, 0, C_NB_FACES);  // draw

	// - draw wall -------------------------------------------------------------
	gui->cubeShader->setInt("blockId", Block::DURABLE_WALL);  // set block type
	// board side 0
	gui->cubeShader->setVec3("blockSize",
		{gui->gameInfo.gameboard.x + 1.0f, 2.0f, 1.0f});
	pos = glm::vec3(0.0f, -1.0f, gui->gameInfo.gameboard.y);
	model = glm::translate(glm::mat4(1.0), pos);
	gui->cubeShader->setMat4("model", model);
	glDrawArrays(GL_POINTS, 0, C_NB_FACES);  // draw

	// board side 1
	gui->cubeShader->setVec3("blockSize",
		{1.0f, 2.0f, gui->gameInfo.gameboard.y + 1.0f});
	pos = glm::vec3(gui->gameInfo.gameboard.x, -1.0f, gui->gameInfo.gameboard.y - 1.0f);
	model = glm::translate(glm::mat4(1.0), pos);
	gui->cubeShader->setMat4("model", model);
	glDrawArrays(GL_POINTS, 0, C_NB_FACES);  // draw

	// board side 2
	gui->cubeShader->setVec3("blockSize",
		{gui->gameInfo.gameboard.x + 1.0f, 2.0f, 1.0f});
	pos = glm::vec3(-1.0f, -1.0f, -1.0f);
	model = glm::translate(glm::mat4(1.0), pos);
	gui->cubeShader->setMat4("model", model);
	glDrawArrays(GL_POINTS, 0, C_NB_FACES);  // draw

	// board side 3
	gui->cubeShader->setVec3("blockSize",
		{1.0f, 2.0f, gui->gameInfo.gameboard.y + 1.0f});
	pos = glm::vec3(-1.0f, -1.0f, gui->gameInfo.gameboard.y);
	model = glm::translate(glm::mat4(1.0), pos);
	gui->cubeShader->setMat4("model", model);
	glDrawArrays(GL_POINTS, 0, C_NB_FACES);  // draw


	// -- draw player ----------------------------------------------------------
	gui->cubeShader->setVec3("blockSize", {1.0f, 1.0f, 1.0f});
	if (gui->gameInfo.player != VOID_POS) {
		// set block type
		gui->cubeShader->setInt("blockId", Block::PLAYER);
		// set block pos
		pos = glm::vec3(gui->gameInfo.player[0], 0.0f, gui->gameInfo.player[1]);
		model = glm::translate(glm::mat4(1.0), pos);
		gui->cubeShader->setMat4("model", model);
		glDrawArrays(GL_POINTS, 0, C_NB_FACES);  // draw
	}

	// -- draw bomb ------------------------------------------------------------
	gui->cubeShader->setVec3("blockSize", {1.0f, 1.0f, 1.0f});
	// set block type
	gui->cubeShader->setInt("blockId", Block::BOMB);
	// set block pos
	pos = glm::vec3(10, 0.0f, 5);
	model = glm::translate(glm::mat4(1.0), pos);
	gui->cubeShader->setMat4("model", model);
	glDrawArrays(GL_POINTS, 0, C_NB_FACES);  // draw

	// -- draw durable wall ----------------------------------------------------
	gui->cubeShader->setVec3("blockSize", {1.0f, 1.0f, 1.0f});
	// set block type
	gui->cubeShader->setInt("blockId", Block::DURABLE_WALL);
	// set block pos
	pos = glm::vec3(7, 0.0f, 7);
	model = glm::translate(glm::mat4(1.0), pos);
	gui->cubeShader->setMat4("model", model);
	glDrawArrays(GL_POINTS, 0, C_NB_FACES);  // draw

	// -- draw durable wall ----------------------------------------------------
	gui->cubeShader->setVec3("blockSize", {1.0f, 1.0f, 1.0f});
	// set block type
	gui->cubeShader->setInt("blockId", Block::DESTRUCTIBLE_WALL);
	// set block pos
	pos = glm::vec3(7, 0.0f, 12);
	model = glm::translate(glm::mat4(1.0), pos);
	gui->cubeShader->setMat4("model", model);
	glDrawArrays(GL_POINTS, 0, C_NB_FACES);  // draw
}

// -- Exceptions errors --------------------------------------------------------

SceneGame::GameException::GameException()
: std::runtime_error("SceneGame Exception") {}

SceneGame::GameException::GameException(const char* what_arg)
: std::runtime_error(std::string(std::string("GameError: ") + what_arg).c_str()) {}
