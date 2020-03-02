#include <stdlib.h>
#include <time.h>
#include <dlfcn.h>
#include <iostream>

#include "bomberman.hpp"

int start(int ac, char const **av) {
	(void)ac;
	(void)av;
	initLogs();  // init logs functions
	initSettings("assets/settings.json");
	initUserData(s.s("userDataFilename"));

	return EXIT_SUCCESS;
}

int main(int ac, char const **av) {
	int ret = start(ac, av);

	if (ret == EXIT_FAILURE) {
		return ret;
	}
	logDebug("running game");

	saveUserData(s.s("userDataFilename"));
	return ret;
}
