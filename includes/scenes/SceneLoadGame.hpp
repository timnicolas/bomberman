#pragma once

#include "ASceneMenu.hpp"

/**
 * @brief menu for game over
 */
class SceneLoadGame : public ASceneMenu {
	public:
		// Constructors
		SceneLoadGame(Gui * gui, float const &dtTime);
		virtual ~SceneLoadGame();
		SceneLoadGame(SceneLoadGame const &src);

		// Operators
		SceneLoadGame &operator=(SceneLoadGame const &rhs);

		// Methods
		virtual bool		init();
		virtual void		load();
		virtual bool		update();

	protected:
		/**
		 * @brief All buttons states
		 */
		struct ButtonsStates {
			int64_t		selectedGame;  /**< Selected game */
			bool		restart;  /**< True if we clicked on the button restart */
			bool		menu;  /**< True if we clicked on the button menu */
			bool		loadGame;  /**< True if we clicked on the button loadGame */
			bool		deleteGame;  /**< True if we clicked on the button deleteGame */
		};
		ButtonsStates	_states;  /**< All buttons states */
		/**
		 * @brief All information & UI of a saved game (from a json file)
		 */
		struct GameSaved {
			uint32_t		gameID;  /**< Game ID */
			SettingsJson	*game;  /**< Game informations (json) */
			std::string		filename;  /**< Filename for game */
			ABaseUI			*ui;  /**< UI element for a game */
		};
		std::vector<GameSaved *>	_gamesSaved;  /**< All game saved information */
		/**
		 * @brief All UI for preview a saved game
		 */
		struct PreviewGame {
			ABaseUI *	title;  /**< UI for title element */
			ABaseUI *	date;  /**< UI for date element */
			ABaseUI *	gameDifficulty;  /**< UI for gameDifficulty element */
			ABaseUI *	levelsDone;  /**< UI for levelsDone element */
			ABaseUI *	scoreTotal;  /**< UI for scoreTotal element */
			ABaseUI *	loadGame;  /**< UI for loadGame element */
			ABaseUI *	deleteGame;  /**< UI for deleteGame element */
		};
		PreviewGame					previewGameUI;  /**< All UI for preview a saved game */
		int64_t						_selectedGame;  /**< Current selected game */
		std::string					_lastSceneName;  /**< Last scene name (to load it when cancel) */

	private:
		SceneLoadGame();
};
