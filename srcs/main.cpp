#include <stdlib.h>
#include <time.h>
#include <dlfcn.h>
#include <iostream>

#include "bomberman.hpp"
#include "Gui.hpp"

int start(int ac, char const **av) {
	(void)ac;
	(void)av;
	initLogs();  // init logs functions
	initSettings("bomberman-assets/settings.json");
	initUserData(s.s("userDataFilename"));

	return EXIT_SUCCESS;
}

bool	exampleGui() {
	GameInfo	gameInfo;
	Gui			gui(gameInfo);

	if (!gui.init()) {
		return false;
	}

	while (true) {
		gui.updateInput();

		if (gameInfo.quit) {
			logInfo("exit game")
			break;
		}

		if (!gui.draw()) {
			return EXIT_FAILURE;
		}
	}

	return true;
}

int main(int ac, char const **av) {
	int ret = start(ac, av);

	if (ret == EXIT_FAILURE) {
		return ret;
	}
	logDebug("running " << "game");
	userData.u("highScore")++;

	saveUserData(s.s("userDataFilename"));

	if (!exampleGui()) {
		return EXIT_FAILURE;
	}

	return ret;
}
