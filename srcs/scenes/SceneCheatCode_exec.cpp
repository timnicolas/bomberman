#include "SceneCheatCode.hpp"
#include "SceneGame.hpp"
#include "Player.hpp"
#include "Bonus.hpp"

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
					this->logerr("Unexpected error, player doesnt exist", false, true);
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
		bool isFly = (args[1] == "enemyFly") ? 1 : 0;
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