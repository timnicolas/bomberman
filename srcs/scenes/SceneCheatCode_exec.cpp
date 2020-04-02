#include "SceneCheatCode.hpp"
#include "SceneGame.hpp"

bool SceneCheatCode::_execHelp(std::vector<std::string> const & args) {
	(void)args;
	_addLine("help");
	SceneManager::openCheatCodeForTime(s.j("cheatcode").u("timeLineShow"));
	return true;  // exit command line after this
}

bool SceneCheatCode::_execClear(std::vector<std::string> const & args) {
	(void)args;
	_addLine("clear");
	SceneManager::openCheatCodeForTime(s.j("cheatcode").u("timeLineShow"));
	return true;  // exit command line after this
}
