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

	_gui->cam->pos = {
		size.x / 2,
		12.0f + (size.x > size.y ? size.x : size.y) * 0.6,
		size.y * 1.3
	};
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

/**
 * @brief Init the json for a level
 *
 * @param levelId The level ID
 * @return false If failed
 */
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
		// this is outside of the platform (no floor)
		lvl->j("objects").add<std::string>("outside", ".");
		// replaced by a crispy wall or nothing (random)
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

	floor.clear();
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
			AEntity *entity;
			while (element != row.end()) {
				entity = *element;
				row.erase(element);
				delete entity;
				element = row.begin();
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
	floor = std::vector< std::vector< bool > >(size.x,
			std::vector< bool >(size.y, true));
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

	_terrain->transform.setPos({5, -2, 0});

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
				if (name == "outside") {
					floor[i][j] = false;  // mo floor -> outside the platform
				}
				else if (insertEntity(name, {i, j}, false, lvl.u("wallGenPercent")) == false)
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
		spawnerEntity->init();
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
