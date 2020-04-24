#include "SceneVictory.hpp"
#include "SceneGame.hpp"
#include "SceneCheatCode.hpp"

SceneVictory::SceneVictory(Gui * gui, float const &dtTime)
: ASceneMenu(gui, dtTime),
  _lastSceneName(SceneNames::MAIN_MENU)
{
	_draw3dMenu = false;
}

SceneVictory::SceneVictory(SceneVictory const & src)
: ASceneMenu(src)
{
	*this = src;
}

SceneVictory::~SceneVictory() {}

SceneVictory & SceneVictory::operator=(SceneVictory const & rhs) {
	if (this != &rhs) {
		logWarn("you are copying SceneVictory")
	}
	return *this;
}

/**
 * @brief init the menu
 *
 * @return true if the init succeed
 * @return false if the init failed
 */
bool			SceneVictory::init() {
	glm::vec2 winSz = _gui->gameInfo.windowSize;
	glm::vec2 tmpPos;
	glm::vec2 tmpSize;
	float menuWidth = winSz.x / 2;
	float menuHeight = winSz.y / 14;
	float statisticHeight = menuHeight * 0.6;

	try {
		tmpPos.x = (winSz.x / 2) - (menuWidth / 2);
		tmpPos.y = winSz.y - menuHeight * 2;
		tmpSize.x = menuWidth;
		tmpSize.y = statisticHeight;
		addTitle(tmpPos, tmpSize, "Victory !");

		try {
			SceneGame & scGame = *reinterpret_cast<SceneGame *>(SceneManager::getScene(SceneNames::GAME));
			statistics.level = &addText(
				{tmpPos.x - 30, tmpPos.y + 30},
				tmpSize,
				"LVL " + std::to_string(scGame.score.getLevelId()))
					.setTextAlign(TextAlign::LEFT);
			tmpPos.y -= statisticHeight;
			std::vector<Score::Stat> scoreStat;
			scGame.score.getStats(scoreStat);
			for (auto &&stat : scoreStat) {
				tmpPos.y -= statisticHeight;
				if (stat.image.size()) {
					statistics.stats.push_back(&addImage({tmpPos.x, tmpPos.y}, {32, 32}, stat.image));
					statistics.stats.push_back(&addText({tmpPos.x + 32, tmpPos.y}, tmpSize, stat.label)
						.setTextAlign(TextAlign::LEFT));
				} else {
					statistics.stats.push_back(&addText(tmpPos, tmpSize, stat.label)
						.setTextAlign(TextAlign::LEFT));
				}
				if (stat.points.size()) {
					statistics.stats.push_back(&addText(tmpPos, tmpSize, stat.points)
						.setTextAlign(TextAlign::RIGHT));
				}
			}
		} catch (ABaseUI::UIException const & e) {
			logErr(e.what());
		}

		tmpSize.y = menuHeight;
		tmpPos.y -= menuHeight * 1.8;
		addButton(tmpPos, tmpSize, "next   level")
			.setKeyLeftClickInput(InputType::CONFIRM)
			.addButtonLeftListener(&_states.nextLevel);

		tmpPos.y -= menuHeight * 1.3;
		addButton(tmpPos, tmpSize, "restart")
			.addButtonLeftListener(&_states.restart);

		tmpPos.y -= menuHeight * 1.3;
		addButton(tmpPos, tmpSize, "main   menu")
			.setKeyLeftClickInput(InputType::GOTO_MENU)
			.addButtonLeftListener(&_states.menu);

		tmpPos.y -= menuHeight * 1.3;
		addButton(tmpPos, tmpSize, "exit")
			.setKeyLeftClickInput(InputType::CANCEL)
			.addButtonLeftListener(&_states.exit);

		tmpSize.x = tmpSize.x * 1.3;
		tmpSize.y = winSz.y - tmpPos.y;
		tmpPos.x = (winSz.x / 2) - ((menuWidth * 1.3) / 2);
		tmpPos.y -= menuHeight * 0.5;
		addRect(tmpPos, tmpSize);

		_initBG();
	}
	catch (ABaseUI::UIException const & e) {
		logErr(e.what());
		return false;
	}
	return true;
}

/**
 * @brief this is the update function (called every frames)
 *
 * @return true if the update is a success
 * @return false if there are an error in update
 */
bool	SceneVictory::update() {
	ASceneMenu::update();
	SceneGame & scGame = *reinterpret_cast<SceneGame *>(SceneManager::getScene(SceneNames::GAME));
	if (s.j("debug").b("3d-menu")) {
		scGame.updateForMenu();
	}
	statistics.level->setText("LVL " + std::to_string(scGame.score.getLevelId()));
	// statistics.score->setText("Score: " + scGame.score.toString());

	std::vector<ABaseUI *>::iterator	it = statistics.stats.begin();
	while (it != statistics.stats.end()) {
		std::vector<Score::Stat> scoreStat;
		scGame.score.getStats(scoreStat);
		for (auto &&stat : scoreStat) {
			if (stat.image.size()) {
				it++;
				if (it == statistics.stats.end())
					break;
			}
			(*it)->setText(stat.label);
			it++;
			if (it == statistics.stats.end())
				break;
			if (stat.points.size()) {
				(*it)->setText(stat.points);
				it++;
				if (it == statistics.stats.end())
					break;
			}
		}
	}

	if (_states.nextLevel) {
		_states.nextLevel = false;
		if (scGame.level + 1 < static_cast<int32_t>(scGame.getNbLevel())) {
			try {
				// reload the current level
				if (!scGame.loadLevel(scGame.level + 1)) {
					return false;
				}
			} catch (std::exception const &e) {
				SceneCheatCode & scCheatCode = *reinterpret_cast<SceneCheatCode *>(
					SceneManager::getScene(SceneNames::CHEAT_CODE)
				);
				logErr("Error: " << e.what());
				scCheatCode.clearAllLn();
				std::stringstream ss;
				ss << "Level " << scGame.level << ": " << e.what();
				scCheatCode.logerr(ss.str());
				scCheatCode.unlockLevel(scGame.level + 1);
				SceneManager::loadScene(SceneNames::LEVEL_SELECTION);
				return true;
			}
			SceneManager::loadScene(_lastSceneName);
		}
		else {
			SceneManager::loadScene(SceneNames::END_GAME);
		}
	}
	else if (_states.restart) {
		_states.restart = false;
		// reload the current level
		if (!scGame.loadLevel(scGame.level)) {
			return false;
		}
		SceneManager::loadScene(_lastSceneName);
	}
	else if (_states.menu) {
		_states.menu = false;
		SceneManager::loadScene(SceneNames::MAIN_MENU);
	}
	else if (_states.exit) {
		_states.exit = false;
		SceneManager::loadScene(SceneNames::EXIT);
	}
	return true;
}

/**
 * @brief called when the scene is loaded
 */
void SceneVictory::load() {
	ASceneMenu::load();
	if (SceneManager::getSceneName() != SceneNames::EXIT) {
		_lastSceneName = SceneManager::getSceneName();
	}
}

/**
 * @brief this is the draw function (called every frames)
 *
 * @return true if the draw is a success
 * @return false if there are an error in draw
 */
bool SceneVictory::draw() {
	bool ret = true;

	/* 3d background */
	if (s.j("debug").b("3d-menu")) {
		SceneGame & scGame = *reinterpret_cast<SceneGame *>(SceneManager::getScene(SceneNames::GAME));
		ret = scGame.drawVictory();  // draw the game if possible
	}
	ret = ASceneMenu::draw();
	return ret & true;
}
