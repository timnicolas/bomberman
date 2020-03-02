#include <stdlib.h>
#include <unistd.h>
#include <chrono>

#include "bomberman.hpp"
#include "Logging.hpp"

SettingsJson s;
SettingsJson userData;

void	initLogs() {
	// init logging
	#if DEBUG
		logging.setLoglevel(LOGDEBUG);
		logging.setPrintFileLine(LOGWARN, true);
		logging.setPrintFileLine(LOGERROR, true);
		logging.setPrintFileLine(LOGFATAL, true);
	#else
		logging.setLoglevel(LOGINFO);
	#endif
	logDebug("using debug mode");
}

bool	initSettings(std::string const & filename) {
	s.name("settings").description("main settings");

	s.add<std::string>("userDataFilename", "configs/userData.json").disableInFile(true);


	SettingsJson list;
	list.add<uint64_t>("elem", 12);
	s.addList<SettingsJson>("name", list);

	// s.add<SettingsJson>("screen");
	// 	s.j("screen").add<std::string>("name", "nibbler").setDescription("name of the game");
	// 	s.j("screen").add<uint64_t>("fps", 60).setMin(30).setMax(120).setDescription("framerate");
	// 	s.j("screen").add<uint64_t>("width", 1200).setMin(400).setMax(4000).setDescription("width of the screen");
	// 	s.j("screen").add<uint64_t>("height", 800).setMin(400).setMax(4000).disableInFile(true)
	// 		.setDescription("height of the screen /!\\ automatically calculed");

	try {
		if (s.loadFile(filename) == false) {
			// warning when loading settings
			return false;
		}
	}
	catch(SettingsJson::SettingsException const & e) {
		logErr(e.what());
		return false;
	}
	return true;
}

bool	initUserData(std::string const & filename) {
	userData.name("userData").description("all data saved");

	userData.add<uint64_t>("highScore", 0);  // example

	try {
		if (userData.loadFile(filename) == false) {
			// warning when loading settings
			return false;
		}
	}
	catch(SettingsJson::SettingsException const & e) {
		// file doesn't exist, create it at the end
		logDebug("the file " << filename << " doesn't exist for now");
		return false;
	}
	return true;
}

bool	saveUserData(std::string const & filename) {
	try {
		userData.saveToFile(filename);
	}
	catch(SettingsJson::SettingsException const & e) {
		logErr(e.what());
		return false;
	}
	return true;
}

std::chrono::milliseconds getMs() {
	return std::chrono::duration_cast<std::chrono::milliseconds>(
		std::chrono::system_clock::now().time_since_epoch());
}
