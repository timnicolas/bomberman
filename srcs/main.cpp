#include <stdlib.h>
#include <time.h>
#include <dlfcn.h>
#include <iostream>

#include "bomberman.hpp"
#include "Gui.hpp"
#include "Inputs.hpp"

int start(int ac, char const **av) {
	(void)ac;
	(void)av;
	initLogs();  // init logs functions
	initSettings("configs/settings.json");
	initUserData(s.s("userDataFilename"));

	return EXIT_SUCCESS;
}

bool	exampleGui() {
	GameInfo	gameInfo;
	Gui			gui(gameInfo);
	Inputs		inputs;

	if (!gui.init()) {
		return false;
	}

	while (true) {
		inputs.update();
		gui.updateInput(inputs);
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

	if (ret != EXIT_SUCCESS) {
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
