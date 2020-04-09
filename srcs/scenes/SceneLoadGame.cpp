#include <dirent.h>
#include <map>
#include <boost/filesystem.hpp>

#include "FileUtils.hpp"
#include "SceneLoadGame.hpp"
#include "SceneGame.hpp"
#include "Save.hpp"

SceneLoadGame::SceneLoadGame(Gui * gui, float const &dtTime)
: ASceneMenu(gui, dtTime),
  _lastSceneName(SceneNames::MAIN_MENU)
{
	_states.restart = false;
	_states.selectedGame = 0;
	_selectedGame = 0;
}

SceneLoadGame::SceneLoadGame(SceneLoadGame const & src)
: ASceneMenu(src)
{
	*this = src;
}

SceneLoadGame::~SceneLoadGame() {}

SceneLoadGame & SceneLoadGame::operator=(SceneLoadGame const & rhs) {
	if (this != &rhs) {
		logWarn("you are copying SceneLoadGame")
	}
	return *this;
}

/**
 * @brief init the menu
 *
 * @return true if the init succeed
 * @return false if the init failed
 */
bool			SceneLoadGame::init() {
	return true;
}

/**
 * @brief called when the scene is loaded
 */
void SceneLoadGame::load() {
	ASceneMenu::load();

	for (auto &&gameSaved : _gamesSaved) {
		delete gameSaved->game;
	}
	_gamesSaved.clear();

	for (auto it = _buttons.begin(); it != _buttons.end(); it++) {
		delete *it;
	}
	_buttons.clear();

	_states.selectedGame = 0;
	_selectedGame = 0;
	if (SceneManager::getSceneName() != SceneNames::EXIT) {
		_lastSceneName = SceneManager::getSceneName();
	}

	glm::vec2 winSz = _gui->gameInfo.windowSize;
	glm::vec2 tmpPos;
	glm::vec2 tmpSize;
	float menuWidth = winSz.x / 2;
	float menuHeight = winSz.y / 14;

	try {
		tmpPos.x = (winSz.x / 2) - (menuWidth / 2);
		tmpPos.y = winSz.y - menuHeight * 2;
		tmpSize.x = menuWidth;
		tmpSize.y = menuHeight;
		addTitle(tmpPos, tmpSize, "Load   game");
		tmpPos.y -= menuHeight * 1.8;

		// Screen Saved Games
		glm::vec2		savedGamesSize = {menuWidth * (2.0/5.0), menuHeight * 1.3 * 5};
		glm::vec2		savedGamesPos = {tmpPos.x + 0, tmpPos.y - savedGamesSize.y};
		ABaseMasterUI	*savedGames = reinterpret_cast<ABaseMasterUI*>(
			&addScrollbar(savedGamesPos, savedGamesSize).enableVertScroll(true).setEnabled(true)
		);

		// - Get files in savePath by last write time order
		boost::filesystem::path saveDir(s.s("savePath"));
		std::multimap<std::time_t, boost::filesystem::path> result_set;
		if (boost::filesystem::exists(saveDir) && boost::filesystem::is_directory(saveDir)) {
			boost::filesystem::directory_iterator end_iter;
			for (boost::filesystem::directory_iterator dir_iter(saveDir) ; dir_iter != end_iter ; ++dir_iter) {
				if (boost::filesystem::is_regular_file(dir_iter->status())) {
					// insert by -last_write_time to have the correct order
					result_set.insert({-boost::filesystem::last_write_time(dir_iter->path()), *dir_iter});
				}
			}
		} else {
			logErr("Cannot open '" << s.s("savePath") << "'");
		}

		// - Add buttons to corrects files to Saved Games
		int i = 1;
		GameSaved *gameSaved = nullptr;
		for (auto file : result_set) {
			gameSaved = new GameSaved();
			try {
				gameSaved->filename = file.second.string();
				std::string filename = file.second.string();
				gameSaved->game = Save::initJson(gameSaved->filename, false);
				gameSaved->ui = &addButton(
					{0, (savedGamesSize.y) - (menuHeight * i * 1.3)},
					{savedGamesSize.x * 0.8, tmpSize.y},
					gameSaved->game->s("Filename")
				).addButtonLeftValueListener(&_states.selectedGame, i)
				.setColor(colorise(s.j("colors").j("blue-light").u("color")))
				.setBorderColor(colorise(s.j("colors").j("blue").u("color")))
				.setMaster(savedGames).setTextScale(0.5);
				gameSaved->gameID = i;
				_gamesSaved.push_back(gameSaved);
				i++;
			} catch (Save::SaveException &e) {
				delete gameSaved;
			}
		}

		// Screen Game Preview
		glm::vec2		previewSize = {menuWidth * (3.0/5.0), menuHeight * 1.3 * 5};
		glm::vec2		previewPos = {tmpPos.x + savedGamesSize.x, tmpPos.y - previewSize.y};
		ABaseMasterUI	*previewGame = &addScrollbar(previewPos, previewSize);
		i = 2;
		previewGameUI.title = &addText(
			{0, (previewSize.y) - (menuHeight * i * 0.8)},
			{previewSize.x * 0.8, tmpSize.y},
			"Select a game.."
		).setTextFont("text").setTextAlign(TextAlign::LEFT).setMaster(previewGame);
		i++;
		previewGameUI.date = &addText(
			{0, (previewSize.y) - (menuHeight * i * 0.8)},
			{previewSize.x * 0.8, tmpSize.y},
			""
		).setTextFont("text")
		.setTextAlign(TextAlign::LEFT)
		.setTextScale(1.0)
		.setMaster(previewGame);
		i++;
		previewGameUI.levelsDone = &addText(
			{0, (previewSize.y) - (menuHeight * i * 0.8)},
			{previewSize.x * 0.8, tmpSize.y},
			""
		).setTextFont("text").setTextAlign(TextAlign::LEFT).setMaster(previewGame);
		i++;
		previewGameUI.loadGame = &addButton(
			{0, (previewSize.y) - (menuHeight * i * 1.0)},
			{previewSize.x * 0.8, tmpSize.y},
			"load"
		).setTextFont("text")
		.addButtonLeftListener(&_states.loadGame)
		.setMaster(previewGame);
		i++;
		previewGameUI.deleteGame = &addButton(
			{0, (previewSize.y) - (menuHeight * i * 1.0)},
			{previewSize.x * 0.8, tmpSize.y},
			"delete"
		).setTextFont("text")
		.addButtonLeftListener(&_states.deleteGame)
		.setMaster(previewGame);


		addButton(
			{5 * 1.3, 0},
			tmpSize,
			"load   game"
		).setMaster(previewGame);

		tmpPos.y -= savedGamesSize.y;

		tmpPos.y -= menuHeight * 1.3;
		addButton(tmpPos, tmpSize, "main menu")
			.setKeyLeftClickInput(InputType::GOTO_MENU)
			.addButtonLeftListener(&_states.menu);

		tmpSize.x = tmpSize.x * 1.3;
		tmpSize.y = winSz.y - tmpPos.y;
		tmpPos.x = (winSz.x / 2) - ((menuWidth * 1.3) / 2);
		tmpPos.y -= menuHeight * 0.5;
		addRect(tmpPos, tmpSize);

		_initBG();
	}
	catch (ABaseUI::UIException const & e) {
		logErr(e.what());
	}
}

/**
 * @brief this is the update function (called every frames)
 *
 * @return true if the update is a success
 * @return false if there are an error in update
 */
bool	SceneLoadGame::update() {
	ASceneMenu::update();
	SceneGame & scGame = *reinterpret_cast<SceneGame *>(SceneManager::getScene(SceneNames::GAME));

	if (_states.selectedGame != _selectedGame) {
		for (auto &&gameSaved : _gamesSaved) {
			if (gameSaved->gameID == _states.selectedGame) {
				gameSaved->ui->setColor(colorise(s.j("colors").j("blue").u("color")));
				previewGameUI.title->setText(gameSaved->game->s("Filename"));
				std::time_t dateLastmodified = static_cast<std::time_t>(gameSaved->game->i("date_lastmodified"));
				std::tm localTime = *std::localtime(&dateLastmodified);
				std::stringstream ss;
				ss << std::put_time(&localTime, "%d/%m/%Y %Hh%M");
				previewGameUI.date->setText(ss.str());
				std::string lvlDone = "Levels done: ";
				lvlDone += std::to_string(gameSaved->game->lj("levels").list.size());
				previewGameUI.levelsDone->setText(lvlDone);
			}
			if (gameSaved->gameID == _selectedGame) {
				gameSaved->ui->setColor(colorise(s.j("colors").j("blue-light").u("color")));
			}
		}

		_selectedGame = _states.selectedGame;
	}
	if (_states.loadGame && _states.selectedGame != 0) {
		for (auto &&gameSaved : _gamesSaved) {
			if (gameSaved->gameID == _states.selectedGame) {
				Save::loadGame(gameSaved->game->s("Filename"));
				SceneManager::loadScene(SceneNames::LEVEL_SELECTION);
			}
		}
	}
	if (_states.deleteGame && _states.selectedGame != 0) {
		for (auto &&gameSaved : _gamesSaved) {
			if (gameSaved->gameID == _states.selectedGame) {
				file::rm(gameSaved->game->s("Filename"));
				load();
				return true;
			}
		}
	}
	if (_states.restart) {
		_states.restart = false;
		scGame.loadLevel(scGame.level);  // reload the current level
		SceneManager::loadScene(_lastSceneName);
	}
	else if (_states.menu) {
		_states.menu = false;
		SceneManager::loadScene(SceneNames::MAIN_MENU);
	}
	return true;
}
