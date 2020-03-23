#include <stdlib.h>
#include <time.h>
#include <dlfcn.h>
#include <iostream>

#include "bomberman.hpp"
#include "SceneManager.hpp"
#include "FileUtils.hpp"

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

	file::mkdir(CONFIG_DIR);

	initSettings(SETTINGS_FILE);

	return EXIT_SUCCESS;
}

int main(int ac, char const **av) {
	/* init program & load settings */
	int ret = start(ac, av);
	srand(time(NULL));

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

	saveSettings(SETTINGS_FILE);
	return ret;
}
