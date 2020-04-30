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

SceneLoadGame::~SceneLoadGame() {
	for (auto &&gameSaved : _gamesSaved) {
		delete gameSaved->game;
	}
	_gamesSaved.clear();
}

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
	float menuWidth = winSz.x * 0.8;
	float menuHeight = winSz.y / 14;

	try {
		tmpPos.x = (winSz.x / 2) - (menuWidth / 2);
		tmpPos.y = winSz.y - menuHeight * 2;
		tmpSize.x = menuWidth;
		tmpSize.y = menuHeight;
		addTitle(tmpPos, tmpSize, "Load   game");
		tmpPos.y -= menuHeight * 0.8;

		// Screen Saved Games
		glm::vec2		savedGamesSize = {menuWidth * (2.0/5.0), menuHeight * 1.3 * 6};
		glm::vec2		savedGamesPos = {tmpPos.x + 0, tmpPos.y - savedGamesSize.y};
		ABaseMasterUI	*savedGames = reinterpret_cast<ABaseMasterUI*>(
			&addScrollbar(savedGamesPos, savedGamesSize).enableVertScroll(true).setEnabled(true)
		);

		// - Get files in SAVE_DIR by last write time order
		boost::filesystem::path saveDir(SAVE_DIR);
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
			logErr("Cannot open '" << SAVE_DIR << "'");
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
					{
						savedGamesSize.x * 0.05,
						(savedGamesSize.y) - (menuHeight * i * 1.3)
					},
					{savedGamesSize.x * 0.8, tmpSize.y},
					gameSaved->game->s("Filename")
				).addButtonLeftValueListener(&_states.selectedGame, i)
				.setColor(colorise(s.j("colors").j("blue-light").u("color")))
				.setBorderColor(colorise(s.j("colors").j("blue").u("color")))
				.setTextAlign(TextAlign::LEFT)
				.setMaster(savedGames).setTextScale(0.6);
				gameSaved->gameID = i;
				_gamesSaved.push_back(gameSaved);
				i++;
			} catch (Save::SaveException &e) {
				delete gameSaved;
			}
		}

		// Screen Game Preview
		glm::vec2		previewSize = {menuWidth * (3.0/5.0), menuHeight * 1.3 * 6};
		glm::vec2		previewPos = {tmpPos.x + savedGamesSize.x, tmpPos.y - previewSize.y};
		ABaseMasterUI	*previewGame = &addScrollbar(previewPos, previewSize);
		glm::vec2		tempPrevSize = {previewSize.x * 0.8, tmpSize.y};
		glm::vec2		tempPrevPos = {(previewSize.x / 2) - (previewSize.x * 0.8 / 2), previewSize.y - menuHeight * 1.3};

		previewGameUI.title = &addText(tempPrevPos, tempPrevSize, "Select a game..")
			.setTextAlign(TextAlign::LEFT).setTextFont("text").setMaster(previewGame);
		tempPrevPos.y -= tmpSize.y * 0.8;
		previewGameUI.date = &addText(tempPrevPos, tempPrevSize, "")
			.setTextFont("text").setTextAlign(TextAlign::LEFT).setMaster(previewGame);
		tempPrevPos.y -= tmpSize.y * 0.8;
		previewGameUI.gameDifficulty = &addText(tempPrevPos, tempPrevSize, "")
			.setTextFont("text").setTextAlign(TextAlign::LEFT).setMaster(previewGame);
		tempPrevPos.y -= tmpSize.y * 0.8;
		previewGameUI.levelsDone = &addText(tempPrevPos, tempPrevSize, "")
			.setTextFont("text").setTextAlign(TextAlign::LEFT).setMaster(previewGame);
		tempPrevPos.y -= tmpSize.y * 0.8;
		previewGameUI.scoreTotal = &addText(tempPrevPos, tempPrevSize, "")
			.setTextFont("text").setTextAlign(TextAlign::LEFT).setMaster(previewGame);
		tempPrevPos.y = tmpSize.y * 2;
		previewGameUI.loadGame = &addButton(tempPrevPos, tempPrevSize, "load")
			// .setTextFont("text")
			.setColor(colorise(s.j("colors").j("blue").u("color")))
			.setBorderColor(colorise(s.j("colors").j("blue").u("color")))
			.addButtonLeftListener(&_states.loadGame)
			.setKeyLeftClickInput(InputType::CONFIRM)
			.setMaster(previewGame);
		tempPrevPos.y -= tmpSize.y * 1.3;
		previewGameUI.deleteGame = &addButton(tempPrevPos, tempPrevSize, "delete")
			// .setTextFont("text")
			.setColor(colorise(s.j("colors").j("red").u("color")))
			.setBorderColor(colorise(s.j("colors").j("red").u("color")))
			.addButtonLeftListener(&_states.deleteGame)
			.setKeyLeftClickScancode(SDL_SCANCODE_DELETE)
			.setMaster(previewGame);

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
				ss << std::put_time(&localTime, "%d/%m/%Y at %H:%M:%S");
				previewGameUI.date->setText(ss.str());
				std::string gameDifficulty = "Difficulty: ";
				if (gameSaved->game->u("difficulty") == 1)
					gameDifficulty += "Hard Core";
				else if (gameSaved->game->u("difficulty") == 2)
					gameDifficulty += "Medium";
				else
					gameDifficulty += "Easy";
				previewGameUI.gameDifficulty->setText(gameDifficulty);
				std::string lvlDone = "Levels done: ";
				lvlDone += std::to_string(gameSaved->game->lj("levels").list.size());
				previewGameUI.levelsDone->setText(lvlDone);
				int64_t scoreTotal = 0;
				for (auto level : gameSaved->game->lj("levels").list) {
					scoreTotal += level->i("score");
				}
				previewGameUI.scoreTotal->setText("Total points: " + std::to_string(scoreTotal));
			}
			if (gameSaved->gameID == _selectedGame) {
				gameSaved->ui->setColor(colorise(s.j("colors").j("blue-light").u("color")));
			}
		}

		_selectedGame = _states.selectedGame;
	}
	if (_states.loadGame) {
		_states.loadGame = false;
		if (_states.selectedGame != 0) {
			for (auto &&gameSaved : _gamesSaved) {
				if (gameSaved->gameID == _states.selectedGame) {
					Save::loadGame(gameSaved->filename);
					SceneManager::loadScene(SceneNames::LEVEL_SELECTION);
				}
			}
		}
	}
	if (_states.deleteGame) {
		_states.deleteGame = false;
		if (_states.selectedGame != 0) {
			for (auto &&gameSaved : _gamesSaved) {
				if (gameSaved->gameID == _states.selectedGame) {
					file::rm(gameSaved->filename);
					load();
					return true;
				}
			}
		}
	}
	if (_states.restart) {
		_states.restart = false;
		scGame.loadLevel(scGame.level);  // reload the current level
		SceneManager::loadScene(_lastSceneName);
	}
	else if (_states.menu || Inputs::getKeyUp(InputType::CANCEL)) {
		_states.menu = false;
		SceneManager::loadScene(SceneNames::MAIN_MENU);
	}
	return true;
}
