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

SceneGame::SceneGame(Gui * gui, float const &dtTime)
: AScene(gui, dtTime) {
	// TODO(ebaudet): init members
}

SceneGame::~SceneGame() {
	delete	_model;
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
	_gui->enableCursor(false);
	// _loadLevel(1);

	try {
		_model = new OpenGLModel(*_gui, "bomberman-assets/3dModels/man.fbx", dtTime,
		animationSpeed);
	}
	catch(OpenGLModel::ModelException const &e) {
		_model = nullptr;
		logErr(e.what())
	}

	return true;
}

// -- Private Methods ----------------------------------------------------------

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

	// test to draw a fbx model
	if (_model) {
		try {
			_model->draw();
		}
		catch(OpenGLModel::ModelException const &e) {
			logErr(e.what())
		}
	}

	// draw skybox
	_gui->drawSkybox(view);

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
		throw SceneException("Map height error");
	for (uint32_t j = 0; j < size.y; j++) {
		std::string line = lvl.lj("map").list[j]->s("line");
		if (line.length() != size.x)
			throw SceneException(("Map width error on line "+std::to_string(j)).c_str());
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
