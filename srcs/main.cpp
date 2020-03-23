#include <stdlib.h>
#include <time.h>
#include <dlfcn.h>
#include <iostream>
#include <vector>

#include "bomberman.hpp"
#include "SceneManager.hpp"
#include "FileUtils.hpp"
#include "Logging.hpp"

/**
 * @brief start the game (init logs & settings)
 *
 * @param ac argc from main
 * @param av argv from main
 * @return int exit value
 */
int start(int ac, char const **av) {
	(void)ac;
	(void)av;
	initLogs();  // init logs functions
	srand(time(NULL));  // init random

	file::mkdir(CONFIG_DIR);  // create config folder
	initSettings(SETTINGS_FILE);  // create settings object

	if (!checkPrgm())  // check validity
		return EXIT_FAILURE;

	return EXIT_SUCCESS;
}

int main(int ac, char const **av) {
	/* init program & load settings */
	int ret = start(ac, av);
	if (ret != EXIT_SUCCESS) {
		return ret;
	}

	/* init the scene manager */
	if (SceneManager::init() == false) {
		return EXIT_FAILURE;
	}

	/* run the game */
	if (SceneManager::run() == false) {
		return EXIT_FAILURE;
	}

	/* save before quit */
	saveSettings(SETTINGS_FILE);
	return ret;
}
