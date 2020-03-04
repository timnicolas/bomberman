#include <stdlib.h>
#include <time.h>
#include <dlfcn.h>
#include <iostream>

#include "bomberman.hpp"
#include "Gui.hpp"
#include "Inputs.hpp"
#include "SceneManager.hpp"

int start(int ac, char const **av) {
	(void)ac;
	(void)av;
	initLogs();  // init logs functions
	initSettings("configs/settings.json");
	initUserData(s.s("userDataFilename"));

	return EXIT_SUCCESS;
}

// bool	exampleGui() {
// 	GameInfo	gameInfo;
// 	Gui			gui(gameInfo);

// 	if (!gui.init()) {
// 		return false;
// 	}

// 	while (true) {
// 		Inputs::update();
// 		gui.updateInput();
// 		if (gameInfo.quit) {
// 			logInfo("exit game")
// 			break;
// 		}

// 		if (!gui.draw()) {
// 			return EXIT_FAILURE;
// 		}
// 	}

// 	return true;
// }

int main(int ac, char const **av) {
	int ret = start(ac, av);

	if (ret != EXIT_SUCCESS) {
		return ret;
	}
	logDebug("running " << "game");
	userData.u("highScore")++;

	// std::cout << s.toString(JsonOpt::VERBOSE | JsonOpt::COLOR) << std::endl;
	// std::cout << s.toString(JsonOpt::DISCARD_DISABLED) << std::endl;

	saveUserData(s.s("userDataFilename"));

	// if (!exampleGui()) {
	// 	return EXIT_FAILURE;
	// }

	SceneManager sceneManager;

	if (sceneManager.init() == false) {
		return EXIT_FAILURE;
	}
	if (sceneManager.run() == false) {
		return EXIT_FAILURE;
	}

	return ret;
}
