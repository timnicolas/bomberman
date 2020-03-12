#include <stdlib.h>
#include <unistd.h>
#include <chrono>

#include "bomberman.hpp"
#include "Logging.hpp"

SettingsJson s;
SettingsJson userData;

/**
 * @brief Init the logs
 *
 * You need to call this function only once at program startup.
 * This function init the logs functions
 */
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

/**
 * @brief Create the pattern for master settings object & load settings
 *
 * @param filename the filename to read to set right values for settings
 * @return true if success
 * @return false if error
 */
bool	initSettings(std::string const & filename) {
	s.name("settings").description("main settings");

	s.add<std::string>("userDataFilename", "configs/userData.json").disableInFile(true);

	s.add<SettingsJson>("screen");
		s.j("screen").add<uint64_t>("fps", 60).setMin(30).setMax(120).setDescription("framerate");

	/* font */
	s.add<SettingsJson>("font");
		s.j("font").add<std::string>("file", "bomberman-assets/fonts/Roboto-Regular.ttf")
			.setDescription("this is the main font");
		s.j("font").add<uint64_t>("size", 40).setMin(10).setMax(50)
			.setDescription("default size for the text");

	/* colors */
	s.add<SettingsJson>("colors");
	// buttons
	s.j("colors").add<SettingsJson>("buttons");
		s.j("colors").j("buttons").add<double>("r", 0.2).setMin(0).setMax(1);
		s.j("colors").j("buttons").add<double>("g", 0.2).setMin(0).setMax(1);
		s.j("colors").j("buttons").add<double>("b", 0.2).setMin(0).setMax(1);
		s.j("colors").j("buttons").add<double>("a", 1.0).setMin(0).setMax(1);

	/* Audio */
	s.add<SettingsJson>("audio");
	s.j("audio").add<double>("Master volume", 1.0).setMin(0.0).setMax(1.0) \
		.setDescription("The global volume of the game.");
	s.j("audio").add<double>("Music volume", 1.0).setMin(0.0).setMax(1.0) \
		.setDescription("The volume of the music.");
	s.j("audio").add<double>("Sound volume", 1.0).setMin(0.0).setMax(1.0) \
		.setDescription("The volume of the sounds effects.");

	s.add<std::string>("mapsPath", "bomberman-assets/maps/").setDescription("folder with all maps");
	/* Graphics */
	s.add<SettingsJson>("graphics");
	s.j("graphics").add<bool>("fullscreen", false).setDescription("Display the game on fullscreen or not.");
	s.j("graphics").add<int64_t>("width", 1600).setMin(800).setMax(2560).setDescription("The resolution's width.");
	s.j("graphics").add<int64_t>("height", 900).setMin(600).setMax(1440).setDescription("The resolution's height.");

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

/**
 * @brief Create the pattern for user data object & load last saved user data
 *
 * @param filename the filename to read to set right values for user data
 * @return true if success
 * @return false if error
 */
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

/**
 * @brief Save the user data
 *
 * @param filename The file to save user data
 * @return true if success
 * @return false if error
 */
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

/**
 * @brief Get the current time in ms
 *
 * @return std::chrono::milliseconds the ms object
 */
std::chrono::milliseconds getMs() {
	return std::chrono::duration_cast<std::chrono::milliseconds>(
		std::chrono::system_clock::now().time_since_epoch());
}
