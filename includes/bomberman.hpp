#pragma once

#ifndef DEBUG
	#define DEBUG false
#endif
/* print log when fps level is too low */
#define DEBUG_FPS_LOW	DEBUG & false  // always false in normal mode
/* open the exit menu before quitting */
#define ASK_BEFORE_QUIT	!DEBUG | true  // always true in normal mode

#include <chrono>
#include "SettingsJson.hpp"
#include "Logging.hpp"

void						initLogs();
bool						initSettings(std::string const & filename);
bool						initUserData(std::string const & filename);
bool						saveUserData(std::string const & filename);
std::chrono::milliseconds	getMs();

/**
 * @brief global variable for general settings
 */
extern SettingsJson s;
/**
 * @brief global variable for all the user specific data
 */
extern SettingsJson userData;
