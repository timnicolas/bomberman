#include <stdlib.h>
#include <unistd.h>
#include <chrono>

#include "bomberman.hpp"
#include "Logging.hpp"
#include "FileUtils.hpp"
#include "Inputs.hpp"

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
		s.s("imgsUI"),
		s.s("loadingImgs"),
		homeDir+CONFIG_DIR,
		homeDir+SAVE_DIR,
	};

	/* list of required files */
	std::vector<std::string> requiredFiles = {
		s.j("fonts").j("base").s("file"),
		s.j("fonts").j("cheatcode").s("file"),
		s.s("loadingSentences"),
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

	s.add<std::string>("userDataFilename", homeDir+USERDATA_FILE).disableInFile(true);

	s.add<SettingsJson>("screen");
		s.j("screen").add<uint64_t>("fps", 60).setMin(30).setMax(120).setDescription("framerate");

	/* font */
	s.add<SettingsJson>("fonts");
		s.j("fonts").add<SettingsJson>("base");
			s.j("fonts").j("base").add<std::string>("file", "bomberman-assets/fonts/snakebold.ttf")
				.setDescription("this is the main font");
			s.j("fonts").j("base").add<uint64_t>("size", 20).setMin(5).setMax(50)
				.setDescription("default size for the text");
		s.j("fonts").add<SettingsJson>("cheatcode");
			s.j("fonts").j("cheatcode").add<std::string>("file", "bomberman-assets/fonts/monaco.ttf")
				.setDescription("this is the font for cheatcode");
			s.j("fonts").j("cheatcode").add<uint64_t>("size", 6).setMin(5).setMax(50)
				.setDescription("default size for the text (cheatcode)");
		s.j("fonts").add<SettingsJson>("text");
			s.j("fonts").j("text").add<std::string>("file", "bomberman-assets/fonts/BalooPaaji2-Regular.ttf")
				.setDescription("this is the main font");
			s.j("fonts").j("text").add<uint64_t>("size", 10).setMin(10).setMax(50)
				.setDescription("default size for the text");

	/* colors */
	s.add<SettingsJson>("colors");
	// buttons
	s.j("colors").add<SettingsJson>("buttons");
		s.j("colors").j("buttons").add<uint64_t>("color", 0x647BCE).setMin(0x000000).setMax(0xFFFFFF);
		s.j("colors").j("buttons").add<uint64_t>("alpha", 0xFF).setMin(0x00).setMax(0xFF);
	s.j("colors").add<SettingsJson>("buttons-disable");
		s.j("colors").j("buttons-disable").add<uint64_t>("color", 0x94A1C7).setMin(0x000000).setMax(0xFFFFFF);
		s.j("colors").j("buttons-disable").add<uint64_t>("alpha", 0xFF).setMin(0x00).setMax(0xFF);
	s.j("colors").add<SettingsJson>("buttons-border");
		s.j("colors").j("buttons-border").add<uint64_t>("color", 0x181818).setMin(0x000000).setMax(0xFFFFFF);
		s.j("colors").j("buttons-border").add<uint64_t>("alpha", 0xFF).setMin(0x00).setMax(0xFF);

	s.j("colors").add<SettingsJson>("red");
		s.j("colors").j("red").add<uint64_t>("color", 0xCF647A).setMin(0x000000).setMax(0xFFFFFF);
		s.j("colors").j("red").add<uint64_t>("alpha", 0xFF).setMin(0x00).setMax(0xFF);
	s.j("colors").add<SettingsJson>("orange");
		s.j("colors").j("orange").add<uint64_t>("color", 0xCF8466).setMin(0x000000).setMax(0xFFFFFF);
		s.j("colors").j("orange").add<uint64_t>("alpha", 0xFF).setMin(0x00).setMax(0xFF);
	s.j("colors").add<SettingsJson>("blue");
		s.j("colors").j("blue").add<uint64_t>("color", 0x647BCE).setMin(0x000000).setMax(0xFFFFFF);
		s.j("colors").j("blue").add<uint64_t>("alpha", 0xFF).setMin(0x00).setMax(0xFF);
	s.j("colors").add<SettingsJson>("blue-light");
		s.j("colors").j("blue-light").add<uint64_t>("color", 0x94A1C7).setMin(0x000000).setMax(0xFFFFFF);
		s.j("colors").j("blue-light").add<uint64_t>("alpha", 0xFF).setMin(0x00).setMax(0xFF);
	s.j("colors").add<SettingsJson>("white");
		s.j("colors").j("white").add<uint64_t>("color", 0xf8f4ef).setMin(0x000000).setMax(0xFFFFFF);
		s.j("colors").j("white").add<uint64_t>("alpha", 0xFF).setMin(0x00).setMax(0xFF);

	// font color
	s.j("colors").add<SettingsJson>("font");
		s.j("colors").j("font").add<uint64_t>("color", 0x181818).setMin(0x000000).setMax(0xFFFFFF);
		s.j("colors").j("font").add<uint64_t>("alpha", 0xFF).setMin(0x00).setMax(0xFF);
	// background color
	s.j("colors").add<SettingsJson>("background");
		s.j("colors").j("background").add<uint64_t>("color", 0x181818).setMin(0x000000).setMax(0xFFFFFF);
		s.j("colors").j("background").add<uint64_t>("alpha", 0xFF).setMin(0x00).setMax(0xFF);
	s.j("colors").add<SettingsJson>("bg-rect");
		s.j("colors").j("bg-rect").add<uint64_t>("color", 0xf8f4ef).setMin(0x000000).setMax(0xFFFFFF);
		s.j("colors").j("bg-rect").add<uint64_t>("alpha", 0x44).setMin(0x00).setMax(0xFF);
	s.j("colors").add<SettingsJson>("bg-rect-border");
		s.j("colors").j("bg-rect-border").add<uint64_t>("color", 0xCF647A).setMin(0x000000).setMax(0xFFFFFF);
		s.j("colors").j("bg-rect-border").add<uint64_t>("alpha", 0xFF).setMin(0x00).setMax(0xFF);

	// collider
	s.j("colors").add<SettingsJson>("collider");
		s.j("colors").j("collider").add<uint64_t>("color", 0x647BCE).setMin(0x000000).setMax(0xFFFFFF);
		s.j("colors").j("collider").add<uint64_t>("alpha", 0xFF).setMin(0x00).setMax(0xFF);

	/* Audio */
	s.add<SettingsJson>("audio");
	s.j("audio").add<double>("master", 1.0).setMin(0.0).setMax(1.0) \
		.setDescription("The global volume of the game.");
	s.j("audio").add<double>("music", 1.0).setMin(0.0).setMax(1.0) \
		.setDescription("The volume of the music.");
	s.j("audio").add<double>("sound", 1.0).setMin(0.0).setMax(1.0) \
		.setDescription("The volume of the sounds effects.");

	/* Images */
	s.add<std::string>("logo", "bomberman-assets/imgs/bomberman-logo.png").setDescription("Logo bomberman");

	/* Folders */
	s.add<std::string>("mapsPath", "bomberman-assets/maps/").setDescription("Folder with all maps");
	s.add<std::string>("imgsUI", "bomberman-assets/imgs/UI/").setDescription("Folder with all UI images");
	s.add<std::string>("loadingImgs", "bomberman-assets/loading/imgs").setDescription("Folder with all loading images");
	s.add<std::string>("loadingSentences", "bomberman-assets/loading/load-sentences.txt")
		.setDescription("File with all loading sentences");

	/* Graphics */
	s.add<SettingsJson>("graphics");
	s.j("graphics").add<bool>("fullscreen", false).setDescription("Display the game on fullscreen or not.");
	s.j("graphics").add<bool>("fitToScreen", false).setDescription("The resolution fit to the screen size");
	s.j("graphics").add<int64_t>("width", 1200).setMin(800).setMax(2560).setDescription("The resolution's width.");
	s.j("graphics").add<int64_t>("height", 800).setMin(600).setMax(1440).setDescription("The resolution's height.");

	/* mouse sensitivity */
	s.add<double>("mouse_sensitivity", 0.1).setMin(0.0).setMax(3.0) \
		.setDescription("Camera mouse sensitivity.");

	/* cheatcode */
	s.add<SettingsJson>("cheatcode").setDescription("All cheat code settings");
		s.j("cheatcode").add<uint64_t>("maxLinesShow", 25).setMin(5).setMax(50).setDescription("number of lines to show");
		s.j("cheatcode").add<uint64_t>("timeLineShow", 5000).setMin(0).setMax(10000)
			.setDescription("Time to show lines before quit");
		s.j("cheatcode").add<uint64_t>("historySize", 1000).setMin(0).setMax(10000)
			.setDescription("size of the history");

	/* Debug */
	s.add<SettingsJson>("debug").setDescription("All debug settings");
		s.j("debug").add<bool>("3d-menu", true).setDescription("Use 3D menu");
		s.j("debug").add<SettingsJson>("show").setDescription("All showables settings for command /debug");
			s.j("debug").j("show").add<bool>("baseBoard", true).setDescription("Show the base board");
			s.j("debug").j("show").add<bool>("terrain", true).setDescription("Show the terrain");
			s.j("debug").j("show").add<bool>("entity", true).setDescription("Show the entities (player & enemy)");
			s.j("debug").j("show").add<bool>("flyHeight", false).setDescription("Show the fly height");
			s.j("debug").j("show").add<bool>("movingCollider", false).setDescription("Show the collider of moving entities");
			s.j("debug").j("show").add<bool>("staticCollider", false).setDescription("Show the collider of static entities");

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
 * @brief Show usage for program
 *
 * @return false Return always false
 */
bool	usage() {
	std::cout << "usage: ./bomberman [-u] [--reset] [--reset-settings] [--reset-history] [--reset-saves]" << std::endl;
	std::cout << "\t" COLOR_BOLD "--reset --reset-all" COLOR_EOC ": "
		"reset all users data (settings, history and saves) before starting" << std::endl;
	std::cout << "\t" COLOR_BOLD "--reset-settings" COLOR_EOC ": "
		"reset all users settings before starting" << std::endl;
	std::cout << "\t" COLOR_BOLD "--reset-history" COLOR_EOC ": "
		"reset commands history before starting" << std::endl;
	std::cout << "\t" COLOR_BOLD "--reset-saves" COLOR_EOC ": "
		"reset all users saves before starting" << std::endl;
	std::cout << "\t" COLOR_BOLD "-u" COLOR_EOC ", " COLOR_BOLD "--usage" COLOR_EOC ": "
		"show usage" << std::endl;
	return false;
}

/**
 * @brief Parse args for program
 *
 * @param nbArgs number of arguments (argc - 1)
 * @param args arguments (av + 1)
 * @return false If need to quit
 */
bool	argparse(int nbArgs, char const ** args) {
	bool	reset_settings = false;
	bool	reset_history = false;
	bool	reset_save = false;
	bool	reset_all = false;
	int		i = 0;
	while (i < nbArgs) {
		if (strcmp(args[i], "--usage") == 0 || strcmp(args[i], "-u") == 0) {
			return usage();
		}
		else if (strcmp(args[i], "--reset-settings") == 0) {
			i++;
			reset_settings = true;
		}
		else if (strcmp(args[i], "--reset-history") == 0) {
			i++;
			reset_history = true;
		}
		else if (strcmp(args[i], "--reset-saves") == 0) {
			i++;
			reset_save = true;
		}
		else if (strcmp(args[i], "--reset-all") == 0 || strcmp(args[i], "--reset") == 0) {
			i++;
			reset_all = true;
		}
		else {
			std::cout << "invalid argument: " << args[i] << std::endl;
			return usage();
		}
		i++;
	}
	if (reset_settings) {
		logInfo("reset all user settings...");
		file::rm(homeDir+SETTINGS_FILE);
		file::rm(Inputs::configFile);
	}
	if (reset_history) {
		logInfo("reset history...");
		file::rm(homeDir+CHEATCODE_HIST_FILE);
	}
	if (reset_save) {
		logInfo("reset all saves...");
		file::rm(homeDir+SAVE_DIR);
	}
	if (reset_all) {
		logInfo("reset saves, settings, and history...");
		file::rm(homeDir+SAVE_DIR);
		file::rm(homeDir+CONFIG_DIR);
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

/**
 * @brief Return time as a string formated m:ss
 *
 * @param time
 * @return std::string formated m:ss
 */
std::string					timeToString(float time) {
	int	minutes = time / 60;
	int	seconds = static_cast<int>(time) % 60;
	std::stringstream ss;
	ss << std::setw(2) << std::setfill('0') << seconds;
	return std::to_string(minutes) + ":" + ss.str();
}

/**
 * @brief Transform color between hexadecimal mode to float vec4.
 *
 * @param color
 * @param alpha
 * @return glm::vec4
 */
glm::vec4					colorise(uint32_t color, uint8_t alpha) {
	float	red = static_cast<float>((color & 0xFF0000) >> 16) / 255.0;
	float	green = static_cast<float>((color & 0x00FF00) >> 8) / 255.0;
	float	blue = static_cast<float>(color & 0x0000FF) / 255.0;
	if (color > 0xffffff) {
		red = 1.0;
		green = 1.0;
		blue = 1.0;
	}
	return (glm::vec4({red, green, blue, (alpha / 255.0)}));
}
