#include <stdlib.h>
#include <time.h>
#include <dlfcn.h>
#include <iostream>

#include "bomberman.hpp"
#include "SceneManager.hpp"

int start(int ac, char const **av) {
	(void)ac;
	(void)av;
	initLogs();  // init logs functions
	initSettings("configs/settings.json");
	initUserData(s.s("userDataFilename"));

	return EXIT_SUCCESS;
}

int main(int ac, char const **av) {
	int ret = start(ac, av);

	if (ret != EXIT_SUCCESS) {
		return ret;
	}

	SceneManager sceneManager;

	if (sceneManager.init() == false) {
		return EXIT_FAILURE;
	}
	if (sceneManager.run() == false) {
		return EXIT_FAILURE;
	}

	saveUserData(s.s("userDataFilename"));
	return ret;
}
