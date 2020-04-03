#include "SceneCheatCode.hpp"
#include "SceneGame.hpp"

bool SceneCheatCode::_execHelp(std::vector<std::string> const & args) {
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
	return true;  // exit command line after this
}

bool SceneCheatCode::_execClear(std::vector<std::string> const & args) {
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
				this->logerr("invalid command argument: " + *arg, false, true);
			}
		}
	}
	else {
		clearAllLn();
	}
	return true;  // exit command line after this
}

bool SceneCheatCode::_execLog(std::vector<std::string> const & args) {
	if (args.size() == 3) {
		if (args[1] == "debug")
			this->logdebug(args[2], false, true);
		else if (args[1] == "info")
			this->loginfo(args[2], false, true);
		else if (args[1] == "success")
			this->logsuccess(args[2], false, true);
		else if (args[1] == "warn")
			this->logwarn(args[2], false, true);
		else if (args[1] == "err")
			this->logerr(args[2], false, true);
		else if (args[1] == "fatal")
			this->logfatal(args[2], false, true);
		else
			this->logerr("invalid log type: " + args[1] + " (/help " + args[0] + ")", false, true);
	}
	else {
		_execHelp({"help", "log"});
		return false;
	}
	SceneManager::openCheatCodeForTime(s.j("cheatcode").u("timeLineShow"));  // show lines for x seconds
	return true;  // exit command line after this
}
