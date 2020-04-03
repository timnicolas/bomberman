#include "SceneCheatCode.hpp"
#include "SceneGame.hpp"

CheatcodeAction::Enum SceneCheatCode::_execHelp(std::vector<std::string> const & args) {
	if (args.size() == 1) {  // only /help
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
			}
		}
	}
	SceneManager::openCheatCodeForTime(s.j("cheatcode").u("timeLineShow"));  // show lines for x seconds
	return CheatcodeAction::CLOSE_RESET;  // exit command line after this
}

CheatcodeAction::Enum SceneCheatCode::_execClear(std::vector<std::string> const & args) {
	if (args.size() > 1) {
		for (auto arg = args.begin() + 1; arg != args.end(); arg++) {
			if (*arg == "history") {
				_cmdHistory.clear();
			}
			else if (*arg == "all") {
				_cmdHistory.clear();
				clearAllLn();
			}
			else {
				this->logerr("Invalid command argument: " + *arg, false, true);
			}
		}
	}
	else {
		clearAllLn();
	}
	return CheatcodeAction::CLOSE_RESET;  // exit command line after this
}

CheatcodeAction::Enum SceneCheatCode::_execLog(std::vector<std::string> const & args) {
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
			return CheatcodeAction::KEEP_OPEN_KEEP_TXT;  // keep command line open
		}
	}
	else {
		_execHelp({"help", "log"});
		SceneManager::openCheatCodeForTime(0);
		return CheatcodeAction::KEEP_OPEN_KEEP_TXT;  // keep command line open
	}
	SceneManager::openCheatCodeForTime(s.j("cheatcode").u("timeLineShow"));  // show lines for x seconds
	return CheatcodeAction::CLOSE_RESET;  // exit command line after this
}

CheatcodeAction::Enum SceneCheatCode::_execTp(std::vector<std::string> const & args) {
	if (args.size() == 3) {
		// bool error;
		// int64_t x = _toInt(args[1], error);
		// if (error) {
		// 	this->logerr("Cannot convert '" + args[1] + "' to integer", false, true);
		// 	return CheatcodeAction::KEEP_OPEN_KEEP_TXT;  // keep command line open
		// }
	}
	else {
		_execHelp({"help", "tp"});
		SceneManager::openCheatCodeForTime(0);
		return CheatcodeAction::KEEP_OPEN_KEEP_TXT;  // keep command line open
	}
	SceneManager::openCheatCodeForTime(s.j("cheatcode").u("timeLineShow"));  // show lines for x seconds
	return CheatcodeAction::CLOSE_RESET;  // exit command line after this
}
