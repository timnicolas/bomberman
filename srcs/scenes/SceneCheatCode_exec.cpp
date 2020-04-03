#include "SceneCheatCode.hpp"
#include "SceneGame.hpp"

bool SceneCheatCode::_execHelp(std::vector<std::string> const & args) {
	(void)args;
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
				_addLine(*cmdName + " is not a valid command", CHEATCODE_TEXT_ERR_COlOR);
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
				_addLine("/clear history to do");  // TODO(tnicolas42) clear history command
			}
			else if (*arg == "all") {
				_addLine("/clear history to do");  // TODO(tnicolas42) clear history command
				clearAllLn();
			}
			else {
				_addLine("invalid command argument: " + *arg, CHEATCODE_TEXT_ERR_COlOR);
			}
		}
	}
	else {
		clearAllLn();
	}
	return true;  // exit command line after this
}
