#pragma once

#ifndef DEBUG
	#define DEBUG false
#endif
#define DEBUG_FPS_LOW	DEBUG & false

#include <chrono>
#include "SettingsJson.hpp"
#include "Logging.hpp"

void						initLogs();
bool						initSettings(std::string const & filename);
bool						initUserData(std::string const & filename);
bool						saveUserData(std::string const & filename);
std::chrono::milliseconds	getMs();

extern SettingsJson s;
extern SettingsJson userData;
