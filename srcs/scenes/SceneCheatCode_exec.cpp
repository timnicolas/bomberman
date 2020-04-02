#include "SceneCheatCode.hpp"
#include "SceneGame.hpp"

bool SceneCheatCode::_execHelp(std::vector<std::string> const & args) {
	(void)args;
	_addLine("help");
	return true;
}

bool SceneCheatCode::_execClear(std::vector<std::string> const & args) {
	(void)args;
	return true;
}
