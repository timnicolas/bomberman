#include <stdlib.h>
#include <unistd.h>
#include <chrono>

#include "bomberman.hpp"
#include "Logging.hpp"
#include "FileUtils.hpp"

SettingsJson s;

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
 * @brief Check if the program is valid (right files, folders, ...)
 *
 * @return false If program is invalid
 */
bool	checkPrgm() {
	/* list of required directories */
	std::vector<std::string> requiredDirs = {
		"bomberman-assets",
		"bomberman-assets/sounds",
		"bomberman-assets/textures",
		"bomberman-assets/skybox",
		s.s("mapsPath"),
	};

	/* list of required files */
	std::vector<std::string> requiredFiles = {
		s.j("font").s("file"),
	};

	for (auto && it : requiredDirs) {
		if (file::isDir(it) == false) {
			logErr(it << " directory doesn't exist");
			return false;
		}
	}
	for (auto && it : requiredFiles) {
		if (file::isFile(it) == false) {
			logErr(it << " file doesn't exist");
			return false;
		}
	}
	return true;
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
		s.j("font").add<uint64_t>("size", 25).setMin(10).setMax(50)
			.setDescription("default size for the text");

	/* colors */
	s.add<SettingsJson>("colors");
	// buttons
	s.j("colors").add<SettingsJson>("buttons");
		s.j("colors").j("buttons").add<double>("r", 0.07).setMin(0).setMax(1);
		s.j("colors").j("buttons").add<double>("g", 0.37).setMin(0).setMax(1);
		s.j("colors").j("buttons").add<double>("b", 0.8).setMin(0).setMax(1);
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
	s.j("graphics").add<int64_t>("width", 1200).setMin(800).setMax(2560).setDescription("The resolution's width.");
	s.j("graphics").add<int64_t>("height", 800).setMin(600).setMax(1440).setDescription("The resolution's height.");

	// mouse sensitivity
	s.add<double>("mouse_sensitivity", 0.1).setMin(0.0).setMax(3.0) \
		.setDescription("Camera mouse sensitivity.");

	try {
		if (file::isDir(filename)) {
			logWarn(filename << " is not the settings file, it is a directory");
		}
		else if (s.loadFile(filename) == false) {
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
bool	saveSettings(std::string const & filename) {
	try {
		if (file::isDir(filename)) {
			logWarn("cannot write in " << filename << " -> it is a directory");
		}
		s.saveToFile(filename);
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
