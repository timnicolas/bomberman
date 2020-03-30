#pragma once

#ifndef DEBUG
	#define DEBUG false
#endif
/* print log when fps level is too low */
#define DEBUG_FPS_LOW	DEBUG & false  // always false in normal mode
/* open the exit menu before quitting */
#define ASK_BEFORE_QUIT	!DEBUG | true  // always true in normal mode
/* show help (shortcuts in buttons) */
#define DEBUG_SHOW_HELP	DEBUG & true  // always false in normal mode

#define CONFIG_DIR		"configs/"
#define SETTINGS_FILE	CONFIG_DIR"settings.json"

#include <chrono>
#include "SettingsJson.hpp"
#include "Logging.hpp"

void						initLogs();
bool						checkPrgm();
bool						initSettings(std::string const & filename);
bool						saveSettings(std::string const & filename);
bool						usage();
bool						argparse(int nbArgs, char const ** args);

std::chrono::milliseconds	getMs();
std::string					timeToString(float time);

/**
 * @brief global variable for general settings
 */
extern SettingsJson s;
