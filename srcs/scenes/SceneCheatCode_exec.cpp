#include "SceneCheatCode.hpp"
#include "SceneGame.hpp"
#include "Player.hpp"
#include "Bonus.hpp"
#include "Save.hpp"
#include "SceneLevelSelection.hpp"
#include "EnemyFly.hpp"
#include "AudioManager.hpp"

int SceneCheatCode::_execHelp(std::vector<std::string> const & args) {
	int success = CheatcodeAction::RESULT_SUCCESS;
	if (args.size() == 1 || (args.size() == 2 && args[1] == "list")) {  // only /help
		_addLine("List of all commands:");
		std::string commands = CHEATCODE_TAB;
		for (auto && it : _commandsList) {
			if (commands != CHEATCODE_TAB) {
				commands += ", ";
			}
			commands += it.first;
		}
		_addLine(commands);
	}
	else {
		for (auto cmdName = args.begin() + 1; cmdName != args.end(); cmdName++) {
			if (_isValidCommand(*cmdName)) {
				std::string ln;
				ln = "/" + *cmdName;
				if (_commandsList[*cmdName].prototype.size() > 0)
					ln += " " + _commandsList[*cmdName].prototype;
				_addLine(ln);
				ln = CHEATCODE_TAB + _commandsList[*cmdName].description;
				_addLine(ln);
			}
			else {
				this->logerr(*cmdName + " is not a valid command", false, true);
				success = CheatcodeAction::RESULT_ERROR;
			}
		}
	}
	return CheatcodeAction::CLOSE | CheatcodeAction::TXT_RESET | CheatcodeAction::CHEAT_TXT_ONLY | success;
}

int SceneCheatCode::_execClear(std::vector<std::string> const & args) {
	int success = CheatcodeAction::RESULT_SUCCESS;
	if (args.size() > 1) {
		for (auto arg = args.begin() + 1; arg != args.end(); arg++) {
			if (*arg == "history") {
				_cmdHistory.clear();
			}
			else if (*arg == "all") {
				_cmdHistory.clear();
				clearAllLn();
			}
			else if (*arg == "list") {
				_addLine("Clear arguments list:\n" CHEATCODE_TAB "all, history");
			}
			else {
				this->logerr("Invalid command argument: " + *arg, false, true);
				success = CheatcodeAction::RESULT_ERROR;
			}
		}
	}
	else {
		clearAllLn();
	}
	return CheatcodeAction::CLOSE | CheatcodeAction::TXT_RESET | success;
}

int SceneCheatCode::_execLog(std::vector<std::string> const & args) {
	int success = CheatcodeAction::RESULT_SUCCESS;
	if (args.size() == 3) {
		if (args[1] == "debug") {
			this->logdebug(args[2], false, true);
		}
		else if (args[1] == "info") {
			this->loginfo(args[2], false, true);
		}
		else if (args[1] == "success") {
			this->logsuccess(args[2], false, true);
		}
		else if (args[1] == "warn") {
			this->logwarn(args[2], false, true);
		}
		else if (args[1] == "err") {
			this->logerr(args[2], false, true);
		}
		else if (args[1] == "fatal") {
			this->logfatal(args[2], false, true);
		}
		else {
			this->logerr("Invalid log type: " + args[1] + " (/help " + args[0] + ")", false, true);
			return CheatcodeAction::KEEP_OPEN | CheatcodeAction::TXT_KEEP | CheatcodeAction::RESULT_ERROR;
		}
	}
	else if (args.size() == 2 && args[1] == "list") {
		_addLine(std::string("List of all possible logs:\n") + CHEATCODE_TAB + "debug, info, success, warn, err, fatal");
		return CheatcodeAction::KEEP_OPEN | CheatcodeAction::TXT_KEEP | CheatcodeAction::RESULT_SUCCESS;
	}
	else {
		_execHelp({"help", args[0]});
		SceneManager::openCheatCodeForTime(0);
		return CheatcodeAction::KEEP_OPEN | CheatcodeAction::TXT_KEEP | CheatcodeAction::RESULT_ERROR;
	}
	return CheatcodeAction::CLOSE | CheatcodeAction::TXT_RESET | CheatcodeAction::CHEAT_TXT_ONLY | success;
}

int SceneCheatCode::_execTp(std::vector<std::string> const & args) {
	int success = CheatcodeAction::RESULT_SUCCESS;
	if (args.size() == 3 || args.size() == 4) {
		bool error;

		/* get x */
		bool xrelative;
		double x = _toFloat(args[1], error, &xrelative);
		if (error) {
			this->logerr("Cannot convert '" + args[1] + "' to float", false, true);
			return CheatcodeAction::KEEP_OPEN | CheatcodeAction::TXT_KEEP | CheatcodeAction::RESULT_ERROR;
		}

		/* get y */
		bool yrelative;
		double y = _toFloat(args[2], error, &yrelative);
		if (error) {
			this->logerr("Cannot convert '" + args[2] + "' to float", false, true);
			return CheatcodeAction::KEEP_OPEN | CheatcodeAction::TXT_KEEP | CheatcodeAction::RESULT_ERROR;
		}

		/* check if we are in game */
		if (SceneManager::getSceneName() != SceneNames::GAME) {
			this->logwarn("You need to be in game to tp", false, true);
			return CheatcodeAction::CLOSE | CheatcodeAction::TXT_RESET | CheatcodeAction::CHEAT_TXT_ONLY
				| CheatcodeAction::RESULT_ERROR;
		}

		/* get real x & y */
		SceneGame & scGame = *reinterpret_cast<SceneGame *>(SceneManager::getScene(SceneNames::GAME));
		glm::vec3 tpPos = glm::vec3(x, 0, y);
		if (xrelative && scGame.player != nullptr)
			tpPos.x += scGame.player->getPos().x;
		if (yrelative && scGame.player != nullptr)
			tpPos.z += scGame.player->getPos().z;

		/* tp */
		if (scGame.player != nullptr && scGame.player->tp(tpPos)) {
			_addLine("tp to " + std::to_string(tpPos.x) + " " + std::to_string(tpPos.z));
		}
		else {
			this->logwarn("Cannot tp at " + std::to_string(tpPos.x) + " " + std::to_string(tpPos.z), false, true);
			success = CheatcodeAction::RESULT_ERROR;
		}
	}
	else {
		_execHelp({"help", args[0]});
		return CheatcodeAction::KEEP_OPEN | CheatcodeAction::TXT_KEEP | CheatcodeAction::RESULT_ERROR;
	}
	return CheatcodeAction::CLOSE | CheatcodeAction::TXT_RESET | CheatcodeAction::CHEAT_TXT_ONLY | success;
}

int SceneCheatCode::_execGetbonus(std::vector<std::string> const & args) {
	int success = CheatcodeAction::RESULT_SUCCESS;
	bool oneSuccess = false;
	if (args.size() >= 2) {
		for (auto arg = args.begin() + 1; arg != args.end(); arg++) {
			if (Bonus::bonus.find(*arg) != Bonus::bonus.end()) {
				if (SceneManager::getSceneName() != SceneNames::GAME) {
					this->logwarn("You need to be in game to get bonus effect", false, true);
					return CheatcodeAction::CLOSE | CheatcodeAction::TXT_RESET | CheatcodeAction::CHEAT_TXT_ONLY
						| CheatcodeAction::RESULT_ERROR;
				}
				SceneGame & scGame = *reinterpret_cast<SceneGame *>(SceneManager::getScene(SceneNames::GAME));
				if (scGame.player != nullptr) {
					scGame.player->takeBonus(Bonus::bonus[*arg]);
					_addLine("Get " + *arg + " bonus effect");
					oneSuccess = true;
				}
				else {
					this->logerr("Unexpected error, player doesn't exist", false, true);
					return CheatcodeAction::CLOSE | CheatcodeAction::TXT_RESET | CheatcodeAction::CHEAT_TXT_ONLY
						| CheatcodeAction::RESULT_ERROR;
				}
			}
			else if (*arg == "list") {
				std::string names = CHEATCODE_TAB;
				for (auto && b : Bonus::bonus) {
					if (names != CHEATCODE_TAB)
						names += ", ";
					names += b.first;
				}
				_addLine("Bonus list:\n" + names);
			}
			else {
				std::string names = CHEATCODE_TAB;
				for (auto && b : Bonus::bonus) {
					if (names != CHEATCODE_TAB)
						names += ", ";
					names += b.first;
				}
				this->logerr("Invalid bonus name.\n" + names);
				success = CheatcodeAction::RESULT_ERROR;
			}
		}
	}
	else {
		_execHelp({"help", args[0]});
		return CheatcodeAction::KEEP_OPEN | CheatcodeAction::TXT_KEEP | CheatcodeAction::RESULT_ERROR;
	}
	if (oneSuccess == false) {
		return CheatcodeAction::KEEP_OPEN | CheatcodeAction::TXT_KEEP | CheatcodeAction::RESULT_ERROR;
	}
	return CheatcodeAction::CLOSE | CheatcodeAction::TXT_RESET | CheatcodeAction::CHEAT_TXT_ONLY | success;
}

int SceneCheatCode::_execLoop(std::vector<std::string> const & args) {
	int success = CheatcodeAction::RESULT_SUCCESS;
	if (args.size() >= 3) {
		bool error;
		uint32_t iter = _toUint(args[1], error);
		if (error) {
			this->logerr("Cannot convert '" + args[1] + "' to int", false, true);
			return CheatcodeAction::KEEP_OPEN | CheatcodeAction::TXT_KEEP | CheatcodeAction::RESULT_ERROR;
		}
		if (iter > 10000) {
			this->logerr("You can't have more than 10000 iterations in a loop");
			return CheatcodeAction::KEEP_OPEN | CheatcodeAction::TXT_KEEP | CheatcodeAction::RESULT_ERROR;
		}
		for (uint32_t i = 0; i < iter; i++) {
			for (auto arg = args.begin() + 2; arg != args.end(); arg++) {
				int res = evalCommand(*arg, true);  // don't add in history
				if (res & CheatcodeAction::RESULT_ERROR) {
					success = CheatcodeAction::RESULT_ERROR;
					this->logerr("Loop exec error. Stoped at iteration " + std::to_string(i)
						+ " on command '" + *arg + "'");
					break;
				}
			}
			if (success & CheatcodeAction::RESULT_ERROR) {
				break;
			}
		}
	}
	else {
		_execHelp({"help", args[0]});
		return CheatcodeAction::KEEP_OPEN | CheatcodeAction::TXT_KEEP | CheatcodeAction::RESULT_ERROR;
	}
	return CheatcodeAction::CLOSE | CheatcodeAction::TXT_RESET | CheatcodeAction::CHEAT_TXT_ONLY | success;
}

int SceneCheatCode::_execExec(std::vector<std::string> const & args) {
	int success = CheatcodeAction::RESULT_SUCCESS;
	if (args.size() >= 2) {
		for (auto arg = args.begin() + 1; arg != args.end(); arg++) {
			int res = evalCommand(*arg, true);  // don't add in history
			if (res & CheatcodeAction::RESULT_ERROR) {
				success = CheatcodeAction::RESULT_ERROR;
				this->logerr("Exec error. Stoped on command '" + *arg + "'");
				break;
			}
		}
	}
	else {
		_execHelp({"help", args[0]});
		return CheatcodeAction::KEEP_OPEN | CheatcodeAction::TXT_KEEP | CheatcodeAction::RESULT_ERROR;
	}
	return CheatcodeAction::CLOSE | CheatcodeAction::TXT_RESET | CheatcodeAction::CHEAT_TXT_ONLY | success;
}

int SceneCheatCode::_execSummon(std::vector<std::string> const & args) {
	int success = CheatcodeAction::RESULT_SUCCESS;

	std::vector<std::string> names = SceneGame::getAllEntityNames();
	if (args.size() == 4) {
		bool error = true;

		/* check name */
		for (auto && name : names) {
			if (args[1] == name)
				error = false;
		}
		if (error) {
			this->logerr("Invalid entity type: " + args[1], false, true);
			return CheatcodeAction::KEEP_OPEN | CheatcodeAction::TXT_KEEP | CheatcodeAction::RESULT_ERROR;
		}

		/* get x */
		bool xrelative;
		int64_t x = _toInt(args[2], error, &xrelative);
		if (error) {
			this->logerr("Cannot convert '" + args[2] + "' to int", false, true);
			return CheatcodeAction::KEEP_OPEN | CheatcodeAction::TXT_KEEP | CheatcodeAction::RESULT_ERROR;
		}

		/* get y */
		bool yrelative;
		int64_t y = _toInt(args[3], error, &yrelative);
		if (error) {
			this->logerr("Cannot convert '" + args[3] + "' to int", false, true);
			return CheatcodeAction::KEEP_OPEN | CheatcodeAction::TXT_KEEP | CheatcodeAction::RESULT_ERROR;
		}

		/* check if we are in game */
		if (SceneManager::getSceneName() != SceneNames::GAME) {
			this->logwarn("You need to be in game to summon entity", false, true);
			return CheatcodeAction::CLOSE | CheatcodeAction::TXT_RESET | CheatcodeAction::CHEAT_TXT_ONLY
				| CheatcodeAction::RESULT_ERROR;
		}

		/* get real x & y */
		SceneGame & scGame = *reinterpret_cast<SceneGame *>(SceneManager::getScene(SceneNames::GAME));
		glm::ivec2 summonPos = glm::ivec2(x, y);
		if (xrelative && scGame.player != nullptr)
			summonPos.x += scGame.player->getIntPos().x;
		if (yrelative && scGame.player != nullptr)
			summonPos.y += scGame.player->getIntPos().y;

		/* summon */
		bool isFly = (args[1] == ENEMY_FLY_STR) ? 1 : 0;
		if (scGame.player != nullptr && scGame.insertEntity(args[1], summonPos, isFly)) {
			_addLine("summon " + args[1] + " at " + std::to_string(summonPos.x) + " " + std::to_string(summonPos.y));
		}
		else {
			this->logwarn("Cannot summon " + args[1] + " at "
				+ std::to_string(summonPos.x) + " " + std::to_string(summonPos.y), false, true);
			success = CheatcodeAction::RESULT_ERROR;
		}
	}
	else if (args.size() == 2 && args[1] == "list") {
		std::string res = CHEATCODE_TAB;
		for (auto && name : names) {
			if (res != CHEATCODE_TAB)
				res += ", ";
			res += name;
		}
		_addLine("List of all entity name:\n" + res);
		return CheatcodeAction::KEEP_OPEN | CheatcodeAction::TXT_KEEP | CheatcodeAction::RESULT_SUCCESS;
	}
	else {
		_execHelp({"help", args[0]});
		return CheatcodeAction::KEEP_OPEN | CheatcodeAction::TXT_KEEP | CheatcodeAction::RESULT_ERROR;
	}
	return CheatcodeAction::CLOSE | CheatcodeAction::TXT_RESET | CheatcodeAction::CHEAT_TXT_ONLY | success;
}

int SceneCheatCode::_execUnlock(std::vector<std::string> const & args) {
	int success = CheatcodeAction::RESULT_SUCCESS;

	std::vector<std::string> names = SceneGame::getAllEntityNames();
	if (args.size() >=2 ) {
		for (uint32_t i = 1; i < args.size(); i++) {
			bool error;
			uint32_t levelId = _toUint(args[i], error);
			if (error) {
				this->logerr("Cannot convert '" + args[i] + "' to unsigned int", false, true);
				return CheatcodeAction::KEEP_OPEN | CheatcodeAction::TXT_KEEP | CheatcodeAction::RESULT_ERROR;
			}
			/* get LevelSelection scene */
			SceneLevelSelection & scLvlSelect = *reinterpret_cast<SceneLevelSelection *>(
				SceneManager::getScene(SceneNames::LEVEL_SELECTION));

			/* check if the level id is valid */
			if (scLvlSelect.getNbLevel() <= levelId) {
				this->logwarn("Invalid level ID: " + args[i], false, true);
				continue;
			}

			/* add to unlocked list */
			if (!_isLevelUnlocked(levelId)) {
				_levelsUnlocked.push_back(levelId);

				/* reload screen if needed */
				if (SceneManager::getSceneName() == SceneNames::LEVEL_SELECTION && scLvlSelect.getCurLevel() == levelId) {
					scLvlSelect.setLevel(0, false);
					scLvlSelect.setLevel(levelId, false);
				}
				_addLine("Level " + std::to_string(levelId) + " unlocked");
			}
		}
		return CheatcodeAction::CLOSE | CheatcodeAction::TXT_DEF | CheatcodeAction::RESULT_SUCCESS;
	}
	else {
		_execHelp({"help", args[0]});
		return CheatcodeAction::KEEP_OPEN | CheatcodeAction::TXT_KEEP | CheatcodeAction::RESULT_ERROR;
	}
	return CheatcodeAction::CLOSE | CheatcodeAction::TXT_RESET | CheatcodeAction::CHEAT_TXT_ONLY | success;
}

int SceneCheatCode::_execRmbonus(std::vector<std::string> const & args) {
	int success = CheatcodeAction::RESULT_SUCCESS;
	bool oneSuccess = false;
	if (args.size() >= 2) {
		for (auto arg = args.begin() + 1; arg != args.end(); arg++) {
			if (Bonus::bonus.find(*arg) != Bonus::bonus.end()) {
				if (SceneManager::getSceneName() != SceneNames::GAME) {
					this->logwarn("You need to be in game to remove a bonus effect", false, true);
					return CheatcodeAction::CLOSE | CheatcodeAction::TXT_RESET | CheatcodeAction::CHEAT_TXT_ONLY
						| CheatcodeAction::RESULT_ERROR;
				}
				SceneGame & scGame = *reinterpret_cast<SceneGame *>(SceneManager::getScene(SceneNames::GAME));
				if (scGame.player != nullptr) {
					scGame.player->rmBonus(Bonus::bonus[*arg]);
					_addLine("Remove " + *arg + " bonus effect");
					oneSuccess = true;
				}
				else {
					this->logerr("Unexpected error, player doesn't exist", false, true);
					return CheatcodeAction::CLOSE | CheatcodeAction::TXT_RESET | CheatcodeAction::CHEAT_TXT_ONLY
						| CheatcodeAction::RESULT_ERROR;
				}
			}
			else if (*arg == "list") {
				std::string names = CHEATCODE_TAB;
				for (auto && b : Bonus::bonus) {
					if (names != CHEATCODE_TAB)
						names += ", ";
					names += b.first;
				}
				_addLine("Bonus list:\n" + names);
			}
			else {
				std::string names = CHEATCODE_TAB;
				for (auto && b : Bonus::bonus) {
					if (names != CHEATCODE_TAB)
						names += ", ";
					names += b.first;
				}
				this->logerr("Invalid bonus name.\n" CHEATCODE_TAB + names, false, true);
			}
		}
	}
	else {
		_execHelp({"help", args[0]});
		return CheatcodeAction::KEEP_OPEN | CheatcodeAction::TXT_KEEP | CheatcodeAction::RESULT_ERROR;
	}
	if (oneSuccess == false) {
		return CheatcodeAction::KEEP_OPEN | CheatcodeAction::TXT_KEEP | CheatcodeAction::RESULT_ERROR;
	}
	return CheatcodeAction::CLOSE | CheatcodeAction::TXT_RESET | CheatcodeAction::CHEAT_TXT_ONLY | success;
}

int SceneCheatCode::_execRestart(std::vector<std::string> const & args) {
	int success = CheatcodeAction::RESULT_SUCCESS;
	if (args.size() == 1) {
		/* check if we are in game */
		if (SceneManager::getSceneName() != SceneNames::GAME) {
			this->logwarn("You need to be in game to restart level", false, true);
			return CheatcodeAction::CLOSE | CheatcodeAction::TXT_RESET | CheatcodeAction::CHEAT_TXT_ONLY
				| CheatcodeAction::RESULT_ERROR;
		}

		/* get scene game */
		SceneGame & scGame = *reinterpret_cast<SceneGame *>(SceneManager::getScene(SceneNames::GAME));
		scGame.loadLevel(scGame.level);
		_addLine("Restart level");
	}
	else {
		_execHelp({"help", args[0]});
		return CheatcodeAction::KEEP_OPEN | CheatcodeAction::TXT_KEEP | CheatcodeAction::RESULT_ERROR;
	}
	return CheatcodeAction::CLOSE | CheatcodeAction::TXT_RESET | CheatcodeAction::CHEAT_TXT_ONLY | success;
}

int SceneCheatCode::_execDebug(std::vector<std::string> const & args) {
	int success = CheatcodeAction::RESULT_SUCCESS;
	bool oneSuccess = false;

	SettingsJson &								debugJson = s.j("debug").j("show");
	std::map<std::string, JsonObj<bool> *> &	debugMap = debugJson.boolMap;

	if (args.size() >= 3) {
		bool value = false;
		bool reset = false;
		if (args[1] == "show") {
			value = true;
		}
		else if (args[1] == "hide") {
			value = false;
		}
		else if (args[1] == "reset") {
			reset = true;
		}
		else {
			this->logerr("Invalid debug type.\n" CHEATCODE_TAB "show, hide, reset", false, true);
			return CheatcodeAction::KEEP_OPEN | CheatcodeAction::TXT_KEEP | CheatcodeAction::RESULT_ERROR;
		}

		for (auto arg = args.begin() + 2; arg != args.end(); arg++) {
			if (*arg == "list") {
				std::string names = CHEATCODE_TAB;
				for (auto && elem : debugMap) {
					if (names != CHEATCODE_TAB)
						names += ", ";
					names += elem.first;
				}
				_addLine("Debug elements list:\n" + names);
				continue;
			}
			try {
				if (reset) {
					if (debugMap.find(*arg) != debugMap.end()) {
						debugMap[*arg]->reset();
						_addLine("Reset " + *arg);
					}
					else {
						throw SettingsJson::SettingsException();
					}
				}
				else {
					debugJson.b(*arg) = value;
					_addLine(std::string(value ? "Show" : "Hide") + " " + *arg);
				}
				oneSuccess = true;
			}
			catch (SettingsJson::SettingsException const & e) {
				std::string names = CHEATCODE_TAB;
				for (auto && elem : debugMap) {
					if (names != CHEATCODE_TAB)
						names += ", ";
					names += elem.first;
				}
				this->logerr("Invalid debug element name.\n" CHEATCODE_TAB + names, false, true);
			}
		}
	}
	else if (args.size() == 2 && args[1] == "list") {
		_addLine("Debug types list:\n" CHEATCODE_TAB "show, hide, reset");
	}
	else if (args.size() == 2 && args[1] == "reset") {
		for (auto && elem : debugMap) {
			elem.second->reset();
		}
		_addLine("Reset all debug elements");
		oneSuccess = true;
	}
	else {
		_execHelp({"help", args[0]});
		return CheatcodeAction::KEEP_OPEN | CheatcodeAction::TXT_KEEP | CheatcodeAction::RESULT_ERROR;
	}
	if (oneSuccess == false) {
		return CheatcodeAction::KEEP_OPEN | CheatcodeAction::TXT_KEEP | CheatcodeAction::RESULT_ERROR;
	}
	return CheatcodeAction::CLOSE | CheatcodeAction::TXT_RESET | CheatcodeAction::CHEAT_TXT_ONLY | success;
}

int SceneCheatCode::_execVolume(std::vector<std::string> const & args) {
	int success = CheatcodeAction::RESULT_SUCCESS;

	SettingsJson &								audioJson = s.j("audio");
	std::map<std::string, JsonObj<double> *> &	audioMap = audioJson.doubleMap;

	if (args.size() == 3) {
		if (args[1] == "list") {
			std::string names = CHEATCODE_TAB;
			for (auto && elem : audioMap) {
				if (names != CHEATCODE_TAB)
					names += ", ";
				names += elem.first;
			}
			_addLine("Audio elements list:\n" + names);
			return CheatcodeAction::KEEP_OPEN | CheatcodeAction::TXT_KEEP | success;
		}
		try {
			if (audioMap.find(args[1]) != audioMap.end()) {
				if (args[2] == "list") {
					_addLine("Audio volume between 0 and 100");
					return CheatcodeAction::KEEP_OPEN | CheatcodeAction::TXT_KEEP | success;
				}
				bool error;
				double val = _toFloat(args[2], error);
				if (error || val < 0 || val > 100) {
					this->logerr("Cannot convert '" + args[1] + "' to number between 0 & 100", false, true);
					return CheatcodeAction::KEEP_OPEN | CheatcodeAction::TXT_KEEP | CheatcodeAction::RESULT_ERROR;
				}
				audioJson.d(args[1]) = val / 100;
				saveSettings(SETTINGS_FILE);
				AudioManager::updateSettings();
				_addLine("Set " + args[1] + " volume level to " + std::to_string(val));
			}
			else {
				throw SettingsJson::SettingsException();
			}
		}
		catch (SettingsJson::SettingsException const & e) {
			std::string names = CHEATCODE_TAB;
			for (auto && elem : audioMap) {
				if (names != CHEATCODE_TAB)
					names += ", ";
				names += elem.first;
			}
			this->logerr("Invalid audio element name.\n" CHEATCODE_TAB + names, false, true);
			return CheatcodeAction::KEEP_OPEN | CheatcodeAction::TXT_KEEP | CheatcodeAction::RESULT_ERROR;
		}
	}
	else if (args.size() == 2 && args[1] == "list") {
		std::string names = CHEATCODE_TAB;
		for (auto && elem : audioMap) {
			if (names != CHEATCODE_TAB)
				names += ", ";
			names += elem.first;
		}
		_addLine("Audio elements list:\n" + names);
		return CheatcodeAction::KEEP_OPEN | CheatcodeAction::TXT_KEEP | success;
	}
	else {
		_execHelp({"help", args[0]});
		return CheatcodeAction::KEEP_OPEN | CheatcodeAction::TXT_KEEP | CheatcodeAction::RESULT_ERROR;
	}
	return CheatcodeAction::CLOSE | CheatcodeAction::TXT_RESET | CheatcodeAction::CHEAT_TXT_ONLY | success;
}
