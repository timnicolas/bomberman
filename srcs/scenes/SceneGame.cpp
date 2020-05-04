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
	AudioManager::loadSound(INTROLEVEL_SOUND);
	AudioManager::loadSound(WIN_SOUND);
	AudioManager::loadSound(GAME_OVER_SOUND);
	AudioManager::loadSound(ALARM_TIME_SOUND);
}

SceneGame::~SceneGame() {
	for (auto &&box : board) {
		for (auto &&row : box) {
			std::vector<AEntity *>::iterator element = row.begin();
			AEntity *entity;
			while (element != row.end()) {
				entity = *element;
				row.erase(element);
				delete entity;
				element = row.begin();
			}
		}
	}
	board.clear();
	for (auto &&box : boardFly) {
		for (auto &&row : box) {
			std::vector<AEntity *>::iterator element = row.begin();
			while (element != row.end()) {
				row.erase(element);
				element = row.begin();
			}
		}
	}
	boardFly.clear();
	if (player != nullptr) {
		delete player;
	}
	std::vector<AEnemy *>::iterator it = enemies.begin();
	AEnemy *enemy;
	while (it != enemies.end()) {
		enemy = *it;
		enemies.erase(it);
		delete enemy;
		it = enemies.begin();
	}
	enemies.clear();

	for (auto it = _mapsList.begin(); it != _mapsList.end(); it++) {
		delete *it;
	}
	_mapsList.clear();

	delete _menuModels.player;
	delete _menuModels.robot;
	delete _menuModels.flower;
	delete _menuModels.fly;
	delete _menuModels.frog;
	delete _terrain;
}

SceneGame::SceneGame(SceneGame const &src)
: ASceneMenu(src) {
	*this = src;
}

// -- Operators ----------------------------------------------------------------

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

	try {
		/* load all models for menu */
		_menuModels.player = new Model(ModelsManager::getModel("white"), getDtTime(),
			ETransform({0, 0, 0}, PLAYER_SIZE));
		_menuModels.player->play = true;
		_menuModels.player->loopAnimation = true;
		_menuModels.player->setAnimation("Armature|idle");

		_menuModels.flower = new Model(ModelsManager::getModel("flower"), getDtTime(),
			ETransform({0, 0, 0}, ENEMY_WITH_EYE_SIZE));
		_menuModels.flower->play = true;
		_menuModels.flower->loopAnimation = true;
		_menuModels.flower->setAnimation("Armature|idle");

		_menuModels.robot = new Model(ModelsManager::getModel("robot"), getDtTime(),
			ETransform({0, 0, 0}, ENEMY_BASIC_SIZE));
		_menuModels.robot->play = true;
		_menuModels.robot->loopAnimation = true;
		_menuModels.robot->setAnimation("Armature|idle");

		_menuModels.fly = new Model(ModelsManager::getModel("flyingBot"), getDtTime(),
			ETransform({0, 0, 0}, ENEMY_FLY_SIZE));
		_menuModels.fly->play = true;
		_menuModels.fly->loopAnimation = true;
		_menuModels.fly->setAnimation("Armature|run");

		_menuModels.frog = new Model(ModelsManager::getModel("frog"), getDtTime(),
		ETransform({0, 0, 0}, ENEMY_FROG_SIZE));
		_menuModels.frog->play = true;
		_menuModels.frog->loopAnimation = true;
		_menuModels.frog->setAnimation("Armature|idle");
		_menuModels.frog->transform.setPos({-2, -1, -4});
		_menuModels.frog->transform.setRot(glm::radians(30.0));

		// init terrain model
		if (!_terrain) {
			_terrain = new Model(ModelsManager::getModel("terrain"), getDtTime(),
			ETransform({0, -2, 0}));
		}

		/* loading sentence */
		allUI.introText = &addText({0, 0}, {_gui->gameInfo.windowSize.x, 70}, "")
			.setTextAlign(TextAlign::CENTER)
			.setColor(colorise(s.j("colors").j("bg-rect").u("color"), s.j("colors").j("bg-rect").u("alpha")))
			.setZ(0.5);
	}
	catch(ModelsManager::ModelsManagerException const &e) {
		logErr(e.what());
		return false;
	}
	catch(OpenGLModel::ModelException const &e) {
		logErr(e.what());
		return false;
	}

	_initGameInfos();

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
 * @param size
 * @return true
 * @return false
 */
bool	SceneGame::positionInGame(glm::vec3 pos, glm::vec3 sz) {
	if (pos.x < 0 || pos.x + sz.x > size.x || pos.z < 0 || pos.z + sz.z > size.y)
		return false;
	return true;
}

// -- AScene Methods -----------------------------------------------------------

bool	SceneGame::updateForMenu() {
	/* set camera position for menu */
	_gui->cam->setMode(CamMode::STATIC);
	_gui->cam->pos = glm::vec3(0, 1.2, 2.5);
	_gui->cam->lookAt(glm::vec3(0, 0.7, 0));
	return true;
}

/**
 * @brief update is called each frame.
 *
 * @return true
 * @return false
 */
bool	SceneGame::update() {
	if (level == NO_LEVEL)
		return true;

	if (_gui->cam->getMode() == CamMode::STATIC)
		_gui->cam->setMode(CamMode::STATIC_DEFPOS);

	_gui->cam->update(_dtTime);

	if (player->getState() == EntityState::DYING)
		state = GameState::GAME_OVER;

	allUI.introText->setEnabled(state == GameState::INTRO);

	if (Inputs::getKeyUp(InputType::CANCEL))
		state = GameState::PAUSE;

	if (state == GameState::PAUSE) {
		AudioManager::pauseAllSounds();
		SceneManager::loadScene(SceneNames::PAUSE);
		return true;
	}
	else if (state == GameState::INTRO) {
		if (_gui->cam->isFollowFinished() || Inputs::getKeyUp(InputType::CONFIRM)) {
			if (Inputs::getKeyUp(InputType::CONFIRM))
				AudioManager::stopSound(INTROLEVEL_SOUND);
			_gui->cam->setMode(CamMode::STATIC_DEFPOS);
			AudioManager::playMusic(musicLevel, 0.3f, true);
			state = GameState::PLAY;
		}
		return true;
	}
	else if (state == GameState::WIN) {
		if (_gui->cam->getMode() != CamMode::FOLLOW_PATH) {
			AudioManager::stopAllSounds();
			try {
				AudioManager::playSound(WIN_SOUND, 1.0f, false, true);
			} catch(Sound::SoundException const & e) {
				logErr(e.what());
			}
			_gui->cam->setMode(CamMode::FOLLOW_PATH);
			_gui->cam->setFollowPath(_getGameOverAnim());
			for (auto &&enemy : enemies) {
				enemy->setState(EntityState::IDLE);
				enemy->update();
			}
		}
		if (_gui->cam->isFollowFinished() || Inputs::getKeyUp(InputType::CONFIRM)) {
			int32_t	crispiesLast = 0;
			for (auto &&box : board) {
				for (auto &&row : box) {
					for (auto &&element : row) {
						if (element->type == Type::CRISPY)
							crispiesLast++;
					}
				}
			}
			score.addBonusTime(levelTime, time);
			uint32_t remainEnemies = 0;
			for (auto && it : enemies) {
				if (it->alive)
					remainEnemies++;
			}
			score.addBonusEnemies(levelEnemies, remainEnemies, levelCrispies, crispiesLast);
			Save::updateSavedFile(*this, true);
			Save::save(true);
			delete player;
			player = nullptr;
			SceneManager::loadScene(SceneNames::VICTORY);
			return true;
		}
	}
	else if (state == GameState::GAME_OVER) {
		if (_gui->cam->getMode() != CamMode::FOLLOW_PATH) {
			AudioManager::stopAllSounds();
			try {
				AudioManager::playSound(GAME_OVER_SOUND, 1.0f, false, true);
			} catch(Sound::SoundException const & e) {
				logErr(e.what());
			}
			_gui->cam->setMode(CamMode::FOLLOW_PATH);
			_gui->cam->setFollowPath(_getGameOverAnim());
			for (auto &&enemy : enemies) {
				enemy->setState(EntityState::IDLE);
				enemy->update();
			}
		}
		if (_gui->cam->isFollowFinished() || Inputs::getKeyUp(InputType::CONFIRM)) {
			// clear game infos.
			player->resetParams();
			Save::updateSavedFile(*this, false);
			Save::save(true);
			delete player;
			player = nullptr;
			SceneManager::loadScene(SceneNames::GAME_OVER);
		}
		return true;
	}

	time += _dtTime;
	if ((levelTime - time) < 0) {
		state = GameState::GAME_OVER;
		return true;
	}
	if ((levelTime - time) < 20) {
		if (!_alarm) {
			try {
				AudioManager::playSound(ALARM_TIME_SOUND);
			} catch(Sound::SoundException const & e) {
				logErr(e.what());
			}
			_alarm = true;
		}
	}

	if (Inputs::getKeyByScancodeUp(SDL_SCANCODE_C)) {
		if (_gui->cam->getMode() == CamMode::FPS) {
			_gui->cam->setMode(CamMode::STATIC_DEFPOS);
		}
		else if (_gui->cam->getMode() == CamMode::STATIC_DEFPOS) {
			_gui->cam->setMode(CamMode::FPS);
		}
	}

	for (auto &&board_it0 : board) {
		for (auto &&board_it1 : board_it0) {
			for (AEntity *board_it2 : board_it1) {
				if (!board_it2->update())
					return false;
			}
		}
	}
	for (auto &&enemy : enemies) {
		if (!enemy->update())
			return false;
	}
	for (auto &&spawner : spawners) {
		if (!spawner->update())
			return false;
	}
	if (!player->update()) {
		return false;
	}

	_updateGameInfos();

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
			continue;
		}
		enemy++;
	}

	for (auto &&board_it0 : board) {
		for (auto &&board_it1 : board_it0) {
			std::vector<AEntity *>::iterator it = board_it1.begin();
			AEntity * copy;
			while (it != board_it1.end()) {
				copy = *it;
				if (!(*it)->postUpdate()) {
					it = board_it1.begin();
					continue;
				}
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
	if (level == NO_LEVEL)
		return drawForMenu();
	return drawGame();
}

/**
 * @brief Draw function if we are in a menu (no level loaded)
 *
 * @return false If failed
 */
bool	SceneGame::drawForMenu() {
	/* draw models */
	try {
		_menuModels.player->transform.setRot(0);
		_menuModels.player->transform.setPos({-0.9, 0, 0});
		if (_menuModels.player->getCurrentAnimationName() != "Armature|idle")
			_menuModels.player->setAnimation("Armature|idle");
		_menuModels.player->draw();

		_menuModels.flower->transform.setRot(0);
		_menuModels.flower->transform.setPos({0.9, 0, 0});
		if (_menuModels.flower->getCurrentAnimationName() != "Armature|idle")
			_menuModels.flower->setAnimation("Armature|idle");
		_menuModels.flower->draw();
	}
	catch(OpenGLModel::ModelException const & e) {
		logErr(e.what());
		return false;
	}

	// draw skybox
	glm::mat4	view = _gui->cam->getViewMatrix();
	_gui->drawSkybox(view);

	return true;
}

/**
 * @brief Draw function if we are in a level
 *
 * @return false If failed
 */
bool	SceneGame::drawGame() {
	if (s.j("debug").j("show").b("terrain")) {
		try {
			_terrain->draw();
		}
		catch(OpenGLModel::ModelException const & e) {
			logErr(e.what());
			return false;
		}
	}

	if (s.j("debug").j("show").b("baseBoard")) {
		// draw floor
		_gui->drawCube(Block::FLOOR, {0.0f, -1.5f, 0.0f}, {size.x, 1.5f, size.y});
	}

	for (auto &&board_it0 : board) {
		for (auto &&board_it1 : board_it0) {
			for (AEntity *board_it2 : board_it1) {
				if (s.j("debug").j("show").b("baseBoard") && !board_it2->draw(*_gui))
					return false;
				if (s.j("debug").j("show").b("staticCollider") && !board_it2->drawCollider())
					return false;
			}
		}
	}

	for (auto &&board_it0 : boardFly) {
		for (auto &&board_it1 : board_it0) {
			for (AEntity *board_it2 : board_it1) {
				if (s.j("debug").j("show").b("flyHeight") && !board_it2->draw(*_gui))
					return false;
				if (s.j("debug").j("show").b("staticCollider") && !board_it2->drawCollider())
					return false;
			}
		}
	}

	for (auto &&enemy : enemies) {
		if (s.j("debug").j("show").b("entity") && !enemy->draw(*_gui))
			return false;
		if (s.j("debug").j("show").b("movingCollider") && !enemy->drawCollider())
			return false;
	}
	if (s.j("debug").j("show").b("entity") && !player->draw(*_gui))
		return false;
	if (s.j("debug").j("show").b("movingCollider") && !player->drawCollider())
		return false;

	// release cubeShader and textures
	_gui->cubeShader->use();
	_gui->textureManager->disableTextures();
	_gui->cubeShader->unuse();

	if (state == GameState::PLAY && allUI.timeLeftImg->getPos() != VOID_SIZE) {
		ASceneMenu::draw();
	}
	else if (state == GameState::INTRO) {
		allUI.introText->draw();
	}

	// draw skybox
	glm::mat4	view = _gui->cam->getViewMatrix();
	_gui->drawSkybox(view);

	return true;
}

/**
 * @brief Draw function if we are in victory menu
 *
 * @return false If failed
 */
bool	SceneGame::drawVictory() {
	/* draw models */
	try {
		_menuModels.player->transform.setRot(0);
		_menuModels.player->transform.setPos({-1, 0, 0});
		if (_menuModels.player->getCurrentAnimationName() != "Armature|dance")
			_menuModels.player->setAnimation("Armature|dance");
		_menuModels.player->draw();

		_menuModels.flower->transform.setRot(0);
		_menuModels.flower->transform.setPos({1, 0, 0});
		if (_menuModels.flower->getCurrentAnimationName() != "Armature|loose")
			_menuModels.flower->setAnimation("Armature|loose");
		_menuModels.flower->draw();
	}
	catch(OpenGLModel::ModelException const & e) {
		logErr(e.what());
		return false;
	}

	// draw skybox
	glm::mat4	view = _gui->cam->getViewMatrix();
	_gui->drawSkybox(view);

	return true;
}

/**
 * @brief Draw function if we are in game over menu
 *
 * @return false If failed
 */
bool	SceneGame::drawGameOver() {
	/* draw models */
	try {
		_menuModels.player->transform.setRot(0);
		_menuModels.player->transform.setPos({-1, 0, 0});
		if (_menuModels.player->getCurrentAnimationName() != "Armature|loose")
			_menuModels.player->setAnimation("Armature|loose");
		_menuModels.player->draw();

		_menuModels.flower->transform.setRot(0);
		_menuModels.flower->transform.setPos({1, 0, 0});
		if (_menuModels.flower->getCurrentAnimationName() != "Armature|dance")
			_menuModels.flower->setAnimation("Armature|dance");
		_menuModels.flower->draw();
	}
	catch(OpenGLModel::ModelException const & e) {
		logErr(e.what());
		return false;
	}

	// draw skybox
	glm::mat4	view = _gui->cam->getViewMatrix();
	_gui->drawSkybox(view);

	return true;
}

/**
 * @brief Draw function if we are in endGame menu
 *
 * @return false If failed
 */
bool	SceneGame::drawEndGame() {
	/* draw simple floor */
	_gui->drawCube(Block::FLOOR, {-10.0, -1.5, -5.0}, {20.0, 0.5, 5.0});

	/* draw models */
	try {
		float tmpX = _menuModels.player->transform.getPos().x;
		if (tmpX < -3 || tmpX > 7)
			tmpX = -3;
		tmpX += 0.03;

		_menuModels.player->animationSpeed = 0.8;
		_menuModels.player->transform.setPos({tmpX, -1, -2});
		_menuModels.player->transform.setRot(glm::radians(90.0));
		if (_menuModels.player->getCurrentAnimationName() != "Armature|run")
			_menuModels.player->setAnimation("Armature|run");
		_menuModels.player->draw();

		tmpX -= 1.3;
		_menuModels.robot->transform.setPos({tmpX, -1, -2});
		_menuModels.robot->transform.setRot(glm::radians(90.0));
		if (_menuModels.robot->getCurrentAnimationName() != "Armature|run")
			_menuModels.robot->setAnimation("Armature|run");
		_menuModels.robot->draw();

		tmpX -= 1.3;
		_menuModels.flower->transform.setPos({tmpX, -1, -2});
		_menuModels.flower->transform.setRot(glm::radians(90.0));
		if (_menuModels.flower->getCurrentAnimationName() != "Armature|run")
			_menuModels.flower->setAnimation("Armature|run");
		_menuModels.flower->draw();

		tmpX -= 1.3;
		_menuModels.fly->transform.setPos({tmpX, .2, -2});
		_menuModels.fly->transform.setRot(glm::radians(90.0));
		if (_menuModels.fly->getCurrentAnimationName() != "Armature|run")
			_menuModels.fly->setAnimation("Armature|run");
		_menuModels.fly->draw();

		_menuModels.frog->draw();
	}
	catch(OpenGLModel::ModelException const & e) {
		logErr(e.what());
		return false;
	}

	// draw skybox
	glm::mat4	view = _gui->cam->getViewMatrix();
	_gui->drawSkybox(view);

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
			AudioManager::playSound(INTROLEVEL_SOUND);
		} catch(Sound::SoundException const & e) {
			logErr(e.what());
		}
	}
	else if (state == GameState::PAUSE
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
	if (_unloadLevel() == false) {
		level = NO_LEVEL;
		return false;
	}
	logDebug("load level " << levelId);
	bool result = _loadLevel(levelId);

	// reinit printed values
	_loadGameInfos();

	_gui->cam->pos = {size.x / 2, 25.0f, size.y * 1.3};
	_gui->cam->lookAt(glm::vec3(
		size.x / 2, 1.0f,
		size.y / 1.9
	));
	_gui->cam->setDefPos();  // set the default position to the current position
	_gui->cam->setMode(CamMode::FOLLOW_PATH);  // set the default camera mode
	_gui->cam->setFollowPath(_getIntroAnim());  // set the follow path
	state = GameState::INTRO;
	AudioManager::pauseMusic();


	/* load sentence */
	std::ifstream	file(s.s("loadingSentences"));
	std::string		line;
	std::vector<std::string> allSentences;
	if (file.is_open()) {
		for (std::string line; std::getline(file, line); ) {
			if (line.size() > 0)
				allSentences.push_back(line);
		}
		file.close();
	}
	else {
		logWarn("unable to load sentences list for loading menu");
	}
	if (allSentences.empty()) {
		logWarn("No sentences in sentences files (" << s.s("loadingSentences") << ")");
		allSentences.push_back("");
	}
	int sentenceID = rand() % allSentences.size();
	allUI.introText->setText(allSentences[sentenceID]);

	// get saved values
	player->resetParams();
	Save::loadStatesSaved(*this);
	if (!player->init()) {
		return false;
	}

	time = 0;
	levelEnemies = enemies.size();

	levelCrispies = 0;
	for (auto &&box : board) {
		for (auto &&row : box) {
			for (auto &&element : row) {
				if (element->type == Type::CRISPY)
					levelCrispies++;
			}
		}
	}

	score.reset();
	score.setLevelId(levelId);

	return result;
}

// -- Private Methods ----------------------------------------------------------

bool	SceneGame::_initJsonLevel(int32_t levelId) {
	// level$(levelID)
	std::stringstream ss;
	ss << "level" << std::setw(2) << std::setfill('0') << levelId;
	std::string		levelName = ss.str();
	// $(mapsPath)/$(levelName).json
	std::string		filename = s.s("mapsPath") + "/" + levelName + ".json";
	if (file::isFile(filename) == false) {
		return false;  // file does not exist
	}

	SettingsJson *	lvl = new SettingsJson();

	lvl->name(levelName).description("Level map");
	lvl->add<std::string>(levelName + "Filename", filename);

	// File json definition:
	lvl->add<std::string>("name");
	lvl->add<std::string>("img", "bomberman-assets/img/icon_level1");
	lvl->add<std::string>("music", "");
	lvl->add<uint64_t>("height", 0).setMin(0).setMax(100);
	lvl->add<uint64_t>("width", 0).setMin(0).setMax(100);
	lvl->add<int64_t>("time", 0).setMin(-1).setMax(86400);
	lvl->add<int64_t>("enemiesToKill", 0).setMin(0).setMax(9999);

	// foreach empty zone, chance to create a wall
	lvl->add<uint64_t>("wallGenPercent", 40).setMin(0).setMax(100);

	lvl->add<SettingsJson>("objects");
		lvl->j("objects").add<std::string>("empty", " ");
		// unique player on game.
		lvl->j("objects").add<std::string>(PLAYER_STR, "p");
		// destructing element dropped by the player.
		lvl->j("objects").add<std::string>(BOMB_STR, "x");
		// indestructible element outside the board
		lvl->j("objects").add<std::string>(WALL_STR, "w");
		// indestructible element of the board
		lvl->j("objects").add<std::string>("block", "b");
		// destructable element, who can give bonuses randomly
		lvl->j("objects").add<std::string>(CRISPY_STR, "c");
		// flag to get end
		lvl->j("objects").add<std::string>(FLAG_STR, "f");
		// end of level when all flag
		lvl->j("objects").add<std::string>(END_STR, "e");
		// no spawn zone
		lvl->j("objects").add<std::string>("safe", "_");
		/* enemies */
		lvl->j("objects").add<std::string>(ENEMY_BASIC_STR, "0");
		lvl->j("objects").add<std::string>(ENEMY_WITH_EYE_STR, "1");
		lvl->j("objects").add<std::string>(ENEMY_FOLLOW_STR, "2");
		lvl->j("objects").add<std::string>(ENEMY_FLY_STR, "3");
		lvl->j("objects").add<std::string>(ENEMY_CRISPY_STR, "4");
		lvl->j("objects").add<std::string>(ENEMY_FROG_STR, "5");

	SettingsJson * spawnerPattern = new SettingsJson();
	spawnerPattern->add<std::string>("typeEnemy", "");
	spawnerPattern->add<uint64_t>("frequency", 5);
	spawnerPattern->add<SettingsJson>("position");
	spawnerPattern->j("position").add<uint64_t>("x");
	spawnerPattern->j("position").add<uint64_t>("y");
	lvl->addList<SettingsJson>("spawner", spawnerPattern);

	SettingsJson * mapPattern = new SettingsJson();
	mapPattern->add<std::string>("0", "");
	mapPattern->add<std::string>("1", "");
	lvl->addList<SettingsJson>("map", mapPattern);

	lvl->add<SettingsJson>("bonus");
		for (auto &&pair : Bonus::bonus) {
			lvl->j("bonus").add<SettingsJson>(pair.first);
			lvl->j("bonus").j(pair.first).add<int64_t>("chance", 0).setMin(0).setMax(100);
			lvl->j("bonus").j(pair.first).add<int64_t>("nb", -1).setMin(-1).setMax(100);
		}
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

/**
 * @brief Unload data of level.
 *
 * @return true if succeed
 * @return false
 */
bool	SceneGame::_unloadLevel() {
	if (level == NO_LEVEL)
		return true;
	logDebug("Unload level " << level);

	for (auto &&box : board) {
		for (auto &&row : box) {
			std::vector<AEntity *>::iterator element = row.begin();
			AEntity *entity;
			while (element != row.end()) {
				entity = *element;
				row.erase(element);
				delete entity;
				element = row.begin();
			}
		}
	}
	board.clear();
	for (auto &&box : boardFly) {
		for (auto &&row : box) {
			for (auto &&element : row) {
				delete element;
			}
		}
	}
	boardFly.clear();
	std::vector<AEnemy *>::iterator it = enemies.begin();
	AEnemy *enemy;
	while (it != enemies.end()) {
		enemy = *it;
		enemies.erase(it);
		delete enemy;
		it = enemies.begin();
	}
	enemies.clear();

	for (auto spawner : spawners) {
		delete spawner;
	}
	spawners.clear();

	// Delete old player
	delete player;
	player = nullptr;

	// spawners
	enemiesToKill = 0;
	enemiesKilled = 0;
	musicLevel = "";

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
	boardFly = std::vector< std::vector<std::vector<AEntity*>> >(size.x,
			   std::vector< std::vector<AEntity*> >(size.y,
			   std::vector< AEntity* >()));

	if (lvl.lj("map").list.size() != size.y)
		throw SceneException("Map height error");

	levelTime = lvl.i("time");

	enemiesToKill = lvl.i("enemiesToKill");

	if (lvl.s("music").size()) {
		musicLevel = lvl.s("music");
	} else {
		musicLevel = AudioManager::musics[rand() % AudioManager::musics.size()];
	}

	AudioManager::loadMusic(musicLevel);

	flags = 0;
	// Get map informations
	for (uint32_t j = 0; j < size.y; j++) {
		// base board creation
		std::string line = lvl.lj("map").list[j]->s("0");

		// throw on bad line width
		if (line.length() != size.x)
			throw SceneException(("Map width error on line "+std::to_string(j)).c_str());

		// loop through line to create the entities
		for (uint32_t i = 0; i < size.x; i++) {
			std::string name;
			for (auto &&entitYCall : entitiesCall) {
				if (line[i] == lvl.j("objects").s(entitYCall.first)[0])
					name = entitYCall.first;
			}
			if (name != "") {
				if (insertEntity(name, {i, j}, false, lvl.u("wallGenPercent")) == false)
					throw SceneException("Unexpected error in map loading");
			}
			else {
				throw SceneException(("Invalid element in map (" + std::to_string(i) + ", "
					+ std::to_string(j) + "): " + line[i]).c_str());
			}
		}

		/* fly board creation */
		line = lvl.lj("map").list[j]->s("1");
		if (line.length() != size.x)
			throw SceneException(("Map fly width error on line "+std::to_string(j)).c_str());
		for (uint32_t i = 0; i < size.x; i++) {
			std::string name;
			for (auto &&entitYCall : entitiesCall) {
				if (line[i] == lvl.j("objects").s(entitYCall.first)[0])
					name = entitYCall.first;
			}
			if (name != "") {
				if (insertEntity(name, {i, j}, true, lvl.u("wallGenPercent")) == false)
					throw SceneException("Unexpected error in map loading");
			}
			else {
				throw SceneException(("Invalid element in map (" + std::to_string(i) + ", "
					+ std::to_string(j) + "): " + line[i]).c_str());
			}
		}
	}

	for (auto spawner : lvl.lj("spawner").list) {
		std::vector<std::string> typeEnemy;
		if (spawner->s("typeEnemy").size() == 0)
			throw SceneException("No type enemies defined in spawner");
		glm::uvec2 spawnerPos = {spawner->j("position").u("x"), spawner->j("position").u("y")};
		if (spawnerPos.x >= size.x || spawnerPos.y >= size.y) {
			throw SceneException("Wrong position for spawner");
		}
		bool	found = false;
		for (auto character : spawner->s("typeEnemy")) {
			found = false;
			for (auto &&entityCall : entitiesCall) {
				if (character == lvl.j("objects").s(entityCall.first)[0]) {
					if (entityCall.second.entityType != EntityType::ENEMY)
						throw SceneException("Spawner only for enemies.");
					typeEnemy.push_back(entityCall.first);
					found = true;
				}
			}
			if (!found)
				throw SceneException("Spawner only for enemies.");
		}
		Spawner *spawnerEntity = new Spawner(*this);
		spawnerEntity->setFrequency(spawner->u("frequency"))
			.setTypeEnemy(typeEnemy)
			.setPos({spawnerPos.x, 0, spawnerPos.y});
		spawners.push_back(spawnerEntity);
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

	_initBonus();

	// set camera
	_gui->cam->lookAt(glm::vec3(size.x / 2 + 0.5f, 1.0f, size.y * 0.7f));

	return true;
}

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

/**
 * @brief Init game informations
 */
void SceneGame::_initGameInfos() {
	try {
		allUI.timeLeftImg = &addImage(VOID_SIZE, VOID_SIZE, "bomberman-assets/textures/bonus/time.png");
		allUI.timeLeftText = &addText(VOID_SIZE, VOID_SIZE, "time-left").setTextAlign(TextAlign::RIGHT);
		allUI.scoreImg = &addImage(VOID_SIZE, VOID_SIZE, "bomberman-assets/textures/bonus/score.png");
		allUI.scoreText = &addText(VOID_SIZE, VOID_SIZE, "score").setTextAlign(TextAlign::RIGHT);
		allUI.lifeImg = &addImage(VOID_SIZE, VOID_SIZE, "bomberman-assets/textures/bonus/life.png");
		allUI.lifeText = &addText(VOID_SIZE, VOID_SIZE, "nb-player-lives").setTextAlign(TextAlign::RIGHT);
		allUI.levelNameText = &addText(VOID_SIZE, VOID_SIZE, "level-name").setTextAlign(TextAlign::RIGHT);
		allUI.enemiesCounterText = &addText(VOID_SIZE, VOID_SIZE, "nb-enemies").setTextAlign(TextAlign::RIGHT);
		allUI.speedImg = &addImage(VOID_SIZE, VOID_SIZE, "bomberman-assets/textures/bonus/speed.png");
		allUI.speedText = &addText(VOID_SIZE, VOID_SIZE, "speed").setTextAlign(TextAlign::RIGHT);
		allUI.bonusBombImg = &addImage(VOID_SIZE, VOID_SIZE, "bomberman-assets/textures/bonus/bomb.png");
		allUI.bonusBombText = &addText(VOID_SIZE, VOID_SIZE, "total-bombs").setTextAlign(TextAlign::RIGHT);
		allUI.bonusFlameImg = &addImage(VOID_SIZE, VOID_SIZE, "bomberman-assets/textures/bonus/flame.png");
		allUI.bonusFlameText = &addText(VOID_SIZE, VOID_SIZE, "bomb-propagation").setTextAlign(TextAlign::RIGHT);
		allUI.bonusFlampassImg = &addImage(VOID_SIZE, VOID_SIZE, "bomberman-assets/textures/bonus/flampass.png");
		allUI.bonusWallpassImg = &addImage(VOID_SIZE, VOID_SIZE, "bomberman-assets/textures/bonus/wallpass.png");
		allUI.bonusDetonatorImg = &addImage(VOID_SIZE, VOID_SIZE, "bomberman-assets/textures/bonus/detonator.png");
		allUI.bonusBombpassImg = &addImage(VOID_SIZE, VOID_SIZE, "bomberman-assets/textures/bonus/bombpass.png");
		allUI.bonusShieldImg = &addImage(VOID_SIZE, VOID_SIZE, "bomberman-assets/textures/bonus/shield.png");
		allUI.bonusShieldText = &addText(VOID_SIZE, VOID_SIZE, "invulnerable").setTextAlign(TextAlign::RIGHT);
	} catch (ABaseUI::UIException const & e) {
		logErr(e.what());
	}
}

/**
 * @brief LoadGameInformations
 */
void		SceneGame::_loadGameInfos() {
	try {
		allUI.timeLeftImg->setPos(VOID_SIZE).setSize(VOID_SIZE);
		allUI.timeLeftText->setPos(VOID_SIZE).setSize(VOID_SIZE);
		allUI.scoreImg->setPos(VOID_SIZE).setSize(VOID_SIZE);
		allUI.scoreText->setPos(VOID_SIZE).setSize(VOID_SIZE);
		allUI.lifeImg->setPos(VOID_SIZE).setSize(VOID_SIZE);
		allUI.lifeText->setPos(VOID_SIZE).setSize(VOID_SIZE);
		allUI.levelNameText->setPos(VOID_SIZE).setSize(VOID_SIZE);
		allUI.enemiesCounterText->setPos(VOID_SIZE).setSize(VOID_SIZE);
		allUI.speedImg->setPos(VOID_SIZE).setSize(VOID_SIZE);
		allUI.speedText->setPos(VOID_SIZE).setSize(VOID_SIZE);
		allUI.bonusBombImg->setPos(VOID_SIZE).setSize(VOID_SIZE);
		allUI.bonusBombText->setPos(VOID_SIZE).setSize(VOID_SIZE);
		allUI.bonusFlameImg->setPos(VOID_SIZE).setSize(VOID_SIZE);
		allUI.bonusFlameText->setPos(VOID_SIZE).setSize(VOID_SIZE);
		allUI.bonusFlampassImg->setPos(VOID_SIZE).setSize(VOID_SIZE);
		allUI.bonusWallpassImg->setPos(VOID_SIZE).setSize(VOID_SIZE);
		allUI.bonusDetonatorImg->setPos(VOID_SIZE).setSize(VOID_SIZE);
		allUI.bonusBombpassImg->setPos(VOID_SIZE).setSize(VOID_SIZE);
		allUI.bonusShieldImg->setPos(VOID_SIZE).setSize(VOID_SIZE);
		allUI.bonusShieldText->setPos(VOID_SIZE).setSize(VOID_SIZE);
	} catch (ABaseUI::UIException const & e) {
		logErr(e.what());
	}
}

/**
 * @brief Update game informations
 */
void			SceneGame::_updateGameInfos() {
	glm::vec2	winSz = _gui->gameInfo.windowSize;
	glm::vec2	tmpPos;
	float		imgY;
	float		textY;
	glm::vec2	tmpSize;
	uint32_t	padding = 5;
	float		menuWidth = winSz.x / 2;
	float		menuHeight = menuWidth / 8;

	try {
		tmpPos.x = (winSz.x / 2) - (menuWidth / 2);
		tmpPos.y = winSz.y - menuHeight * 2;
		imgY = tmpPos.y;
		textY = tmpPos.y + 2;
		tmpSize.x = menuWidth;
		tmpSize.y = menuHeight;
		tmpSize = {32, 32};

		// -- Title ---------
		/* level name */
		allUI.levelNameText->setText(getLevelName(level))
			.setSize(VOID_POS).setCalculatedSize();
		allUI.levelNameText->setPos({(winSz.x / 2) - allUI.levelNameText->getSize().x / 2, tmpPos.y + 1.5 * tmpSize.y});

		tmpPos.x = (winSz.x / 2) - (menuWidth / 2);
		tmpPos.y = winSz.y - menuHeight * 2;

		// -- Top -----------
		/* time left */
		allUI.timeLeftImg->setPos({tmpPos.x, imgY}).setSize(tmpSize);
		tmpPos.x += allUI.timeLeftImg->getSize().x;
		allUI.timeLeftText->setPos({tmpPos.x, textY}).setText(timeToString(levelTime - time))
			.setSize(VOID_POS).setCalculatedSize();
		if (!_alarm)
			allUI.timeLeftText->setTextColor(colorise(s.j("colors").j("font").u("color")));
		else
			allUI.timeLeftText->setTextColor(colorise(s.j("colors").j("orange").u("color")));
		tmpPos.x += allUI.timeLeftText->getSize().x;

		/* life */
		tmpPos.x += padding;
		allUI.lifeImg->setPos({tmpPos.x, imgY}).setSize(tmpSize);
		tmpPos.x += allUI.lifeImg->getSize().x;
		allUI.lifeText->setPos({tmpPos.x, textY}).setText(std::to_string(player->lives))
			.setSize(VOID_SIZE).setCalculatedSize();
		tmpPos.x += allUI.lifeText->getSize().x;

		/* enemies counter */
		if (enemiesToKill > 0) {
			tmpPos.x += padding;
			std::string enemiesStr = std::to_string(enemiesKilled) + "/" + std::to_string(enemiesToKill);
			glm::vec4 color = enemiesKilled >= enemiesToKill
												? colorise(s.j("colors").j("green").u("color"))
												: colorise(s.j("colors").j("red").u("color"));
			allUI.enemiesCounterText->setPos({tmpPos.x, textY}).setText(enemiesStr)
				.setSize(VOID_POS).setCalculatedSize().setTextColor(color);
			tmpPos.x += allUI.enemiesCounterText->getSize().x;
		}

		/* score */
		tmpPos.x = (winSz.x / 2) + (menuWidth / 2);
		allUI.scoreText->setText(score.toString()).setSize(VOID_POS).setCalculatedSize();
		tmpPos.x -= allUI.scoreText->getSize().x;
		allUI.scoreText->setPos({tmpPos.x, textY});
		allUI.scoreImg->setSize(tmpSize);
		tmpPos.x -= allUI.scoreImg->getSize().x;
		allUI.scoreImg->setPos({tmpPos.x, imgY});

		// -- Bonus -----------
		glm::vec2 pos = {winSz.x / 10, winSz.y - menuHeight * 4};
		tmpPos = pos;

		/* speed */
		// tmpPos.x += padding;
		allUI.speedImg->setPos(tmpPos).setSize(tmpSize);
		tmpPos.x += allUI.speedImg->getSize().x;
		std::string	speed = std::to_string(player->speed);
		speed = speed.substr(0, speed.find("."));
		allUI.speedText->setPos({tmpPos.x, tmpPos.y + 2}).setText(speed)
			.setSize(VOID_SIZE).setCalculatedSize();
		pos.y -= menuHeight * 0.8;
		tmpPos = pos;

		/* bonus bomb */
		allUI.bonusBombImg->setPos(tmpPos).setSize(tmpSize);
		tmpPos.x += allUI.bonusBombImg->getSize().x;
		allUI.bonusBombText->setPos({tmpPos.x, tmpPos.y + 2}).setText(std::to_string(player->totalBombs))
			.setSize(VOID_SIZE).setCalculatedSize();
		pos.y -= menuHeight * 0.8;
		tmpPos = pos;

		/* bonus flame */
		allUI.bonusFlameImg->setPos(tmpPos).setSize(tmpSize);
		tmpPos.x += allUI.bonusFlameImg->getSize().x;
		allUI.bonusFlameText->setPos({tmpPos.x, tmpPos.y + 2}).setText(std::to_string(player->bombProgation))
			.setSize(VOID_SIZE).setCalculatedSize();
		pos.y -= menuHeight * 0.8;
		tmpPos = pos;

		/* bonus flampass */
		if (player->passFire) {
			allUI.bonusFlampassImg->setPos(tmpPos).setSize(tmpSize);
			pos.y -= menuHeight * 0.8;
			tmpPos = pos;
		} else {
			allUI.bonusFlampassImg->setPos(VOID_POS).setSize(VOID_SIZE);
		}
		/* bonus wallpass */
		if (player->passWall) {
			allUI.bonusWallpassImg->setPos(tmpPos).setSize(tmpSize);
			pos.y -= menuHeight * 0.8;
			tmpPos = pos;
		} else {
			allUI.bonusWallpassImg->setPos(VOID_POS).setSize(VOID_SIZE);
		}
		/* bonus detonator */
		if (player->detonator) {
			allUI.bonusDetonatorImg->setPos(tmpPos).setSize(tmpSize);
			pos.y -= menuHeight * 0.8;
			tmpPos = pos;
		} else {
			allUI.bonusDetonatorImg->setPos(VOID_POS).setSize(VOID_SIZE);
		}
		/* bonus passBomb */
		if (player->passBomb) {
			allUI.bonusBombpassImg->setPos(tmpPos).setSize(tmpSize);
			pos.y -= menuHeight * 0.8;
			tmpPos = pos;
		} else {
			allUI.bonusBombpassImg->setPos(VOID_POS).setSize(VOID_SIZE);
		}
		/* bonus invulnerable */
		if (player->invulnerable) {
			allUI.bonusShieldImg->setPos(tmpPos).setSize(tmpSize);
			tmpPos.x += allUI.bonusShieldImg->getSize().x;
			std::string	invulnerable = std::to_string(player->invulnerable);
			invulnerable = invulnerable.substr(0, invulnerable.find(".")+2);
			allUI.bonusShieldText->setPos({tmpPos.x, tmpPos.y + 2}).setText(timeToString(player->invulnerable))
				.setSize(VOID_SIZE).setCalculatedSize();
		} else {
			allUI.bonusShieldImg->setPos(VOID_POS).setSize(VOID_SIZE);
			allUI.bonusShieldText->setPos(VOID_POS).setSize(VOID_SIZE);
		}
	} catch (ABaseUI::UIException const & e) {
		logErr(e.what());
	}
}

/**
 * @brief Init the bonus member according to the description in level.json
 *
 * @return true if success
 * @return false if error.
 */
bool			SceneGame::_initBonus() {
	try {
		if (bonus.size())
			bonus.erase(bonus.begin(), bonus.end());
		SettingsJson	&lvl = getSettingsLevel();
		for (auto &&pair : Bonus::bonus) {
			bonus.insert({
				pair.first,
				{
					lvl.j("bonus").j(pair.first).i("chance"),
					lvl.j("bonus").j(pair.first).i("nb")
				}
			});
		}
	} catch (SceneGameException &e) {
		logErr(e.what());
		return false;
	}
	return true;
}

std::vector<CamPoint>	SceneGame::_getIntroAnim() const {
	return {
		{
			{80, 80, 150},  // pos
			CamMovement::NoDirection,  // lookDir
			{size.x / 2, 1, size.y / 2},  // lookAt
			true,  // tpTo
			-1,  // speed
		},
		{
			{size.x / 2, 32, size.y / 2},  // pos
			CamMovement::NoDirection,  // lookDir
			{size.x / 2, 1, size.y / 2},  // lookAt
			false,  // tpTo
			_gui->cam->movementSpeed * 5,  // speed
		},
		{
			{size.x / 2 + 5, 45, size.y / 2 + 5},  // pos
			CamMovement::NoDirection,  // lookDir
			{size.x / 2, 1, size.y / 2},  // lookAt
			false,  // tpTo
			_gui->cam->movementSpeed / 3,  // speed
		},

		{
			{-15, 20, -15},  // pos
			CamMovement::NoDirection,  // lookDir
			{size.x / 2, 1, size.y / 2},  // lookAt
			true,  // tpTo
			-1,  // speed
		},
		{
			{-15, 20, size.y + 15},  // pos
			CamMovement::NoDirection,  // lookDir
			{size.x / 2, 1, size.y / 2},  // lookAt
			false,  // tpTo
			-1,  // speed
		},

		{
			_gui->cam->getDefPos(),  // pos
			CamMovement::NoDirection,  // lookDir
			{size.x / 2, 1, size.y / 2},  // lookAt
			false,  // tpTo
			-1,  // speed
		},
	};
}

std::vector<CamPoint>	SceneGame::_getGameOverAnim() const {
	return {
		{
			_gui->cam->getDefPos(),  // pos
			CamMovement::NoDirection,  // lookDir
			player->position,  // lookAt
			true,  // tpTo
			-1,  // speed
		},
		{
			{player->position.x, player->position.y + 7, player->position.z},  // pos
			CamMovement::NoDirection,  // lookDir
			player->position,  // lookAt
			false,  // tpTo
			-1,  // speed
		},
		{
			{player->position.x, player->position.y + 20, player->position.z + 5},  // pos
			CamMovement::NoDirection,  // lookDir
			player->position,  // lookAt
			false,  // tpTo
			_gui->cam->movementSpeed / 5,  // speed
		},
		{
			{player->position.x, player->position.y + 1, player->position.z},  // pos
			CamMovement::NoDirection,  // lookDir
			player->position,  // lookAt
			false,  // tpTo
			_gui->cam->movementSpeed * 5,  // speed
		},
	};
}

std::vector<CamPoint>	SceneGame::_getVictoryAnim() const {
	return {
		{
			_gui->cam->getDefPos(),  // pos
			CamMovement::NoDirection,  // lookDir
			player->position,  // lookAt
			true,  // tpTo
			-1,  // speed
		},
		{
			{player->position.x, player->position.y + 7, player->position.z},  // pos
			CamMovement::NoDirection,  // lookDir
			player->position,  // lookAt
			false,  // tpTo
			-1,  // speed
		},
		{
			{player->position.x, player->position.y + 20, player->position.z + 5},  // pos
			CamMovement::NoDirection,  // lookDir
			player->position,  // lookAt
			false,  // tpTo
			_gui->cam->movementSpeed / 5,  // speed
		},
		{
			{player->position.x, player->position.y + 1, player->position.z},  // pos
			CamMovement::NoDirection,  // lookDir
			player->position,  // lookAt
			false,  // tpTo
			_gui->cam->movementSpeed * 5,  // speed
		},
	};
}

// -- getter -------------------------------------------------------------------

uint32_t	SceneGame::getNbLevel() const { return _mapsList.size(); }
std::string	SceneGame::getLevelName(int32_t levelId) const {
	if (levelId == NO_LEVEL)
		return "NO_LEVEL";
	if (static_cast<int32_t>(_mapsList.size()) <= levelId)
		throw SceneGameException(("Level " + std::to_string(levelId) + " do not exist.").c_str());
	return _mapsList[levelId]->s("name");
}
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

SceneGame::SceneGameException::SceneGameException()
: std::runtime_error("SceneGame Exception") {}

SceneGame::SceneGameException::SceneGameException(const char* whatArg)
: std::runtime_error(std::string(std::string("SceneGameError: ") + whatArg).c_str()) {}

// -- DrawForMenu struct -------------------------------------------------------
SceneGame::DrawForMenu::DrawForMenu() {
	player = nullptr;
	flower = nullptr;
	robot = nullptr;
}
