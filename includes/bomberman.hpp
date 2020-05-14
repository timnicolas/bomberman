#pragma once

#include <cstdlib>

#ifndef DEBUG
	#define DEBUG false
#endif
/* print log when fps level is too low */
#define DEBUG_FPS_LOW	DEBUG & false  // always false in normal mode
/* open the exit menu before quitting */
#define ASK_BEFORE_QUIT	!DEBUG | true  // always true in normal mode
/* show help (shortcuts in buttons) */
#define DEBUG_SHOW_HELP	DEBUG & true  // always false in normal mode


/**
 *  /!\ The config dirs and files have to be called with `homeDir` to have the
 *  correct path because home path is used.
 *  ex: homeDir+CONFIG_DIR or homeDir+SETTINGS_FILE
 */
#ifdef __APPLE__
	#define CONFIG_DIR				"/Library/Application Support/bomberman/configs/"
#else
	#define CONFIG_DIR				"configs/"
#endif
#define SETTINGS_FILE			CONFIG_DIR"settings.json"
#define CONTROLS_FILE			CONFIG_DIR"controls.json"
#define USERDATA_FILE			CONFIG_DIR"userData.json"
#define CHEATCODE_HIST_FILE		CONFIG_DIR"history.cheatcode"
#ifdef __APPLE__
	#define SAVE_DIR				"/Library/Application Support/bomberman/saves/"
#else
	#define SAVE_DIR				"saves/"
#endif
#define RESTART_TO_UPDATE_RESOLUTION	true  // restart game to reset resolution

#define PLAYER_SIZE			glm::vec3(1.3, 1.3, 1.3)
#define ENEMY_BASIC_SIZE	glm::vec3(0.7, 0.7, 0.7)
#define ENEMY_WITH_EYE_SIZE	glm::vec3(1.5, 1.5, 1.5)
#define ENEMY_FLY_SIZE		glm::vec3(0.7, 0.7, 0.7)
#define ENEMY_FROG_SIZE		glm::vec3(1.0, 1.0, 1.0)
#define ENEMY_CRISPY_SIZE	glm::vec3(1.0, 1.0, 1.0)
#define ENEMY_FOLLOW_SIZE	glm::vec3(1.0, 1.0, 1.0)

#include <chrono>
#include "SettingsJson.hpp"
#include "Logging.hpp"
#include "useGlm.hpp"

#ifdef __APPLE__
	static const std::string	homeDir = getenv("HOME");
#else
	static const std::string	homeDir = "";
#endif
void						initLogs();
bool						checkPrgm();
bool						initSettings(std::string const & filename);
bool						saveSettings(std::string const & filename);
bool						usage();
bool						argparse(int nbArgs, char const ** args);

std::chrono::milliseconds	getMs();
std::string					timeToString(float time);
glm::vec4					colorise(uint32_t color, uint8_t alpha = 0xff);

/**
 * @brief global variable for general settings
 */
extern SettingsJson s;
