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
 * @return int Main exit value
 */
int start(int ac, char const **av) {
	initLogs();  // init logs functions
	srand(time(NULL));  // init random

	if (argparse(ac - 1, av + 1) == false)  // parse arguments
		return EXIT_FAILURE;

	file::mkdir(homeDir+CONFIG_DIR);  // create config folder
	initSettings(homeDir+SETTINGS_FILE);  // create settings object

	file::mkdir(homeDir+SAVE_DIR);

	if (!checkPrgm())  // check validity
		return EXIT_FAILURE;

	return EXIT_SUCCESS;
}

/**
 * @brief This is the main game function, this function execute all the game
 *
 * @param ac argc from main
 * @param av argv from main
 * @return int Main exit value
 */
int bomberman(int ac, char const ** av) {
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
	saveSettings(homeDir+SETTINGS_FILE);
	return ret;
}

int main(int ac, char const ** av) {
	int ret = bomberman(ac, av);
	return ret;
}
