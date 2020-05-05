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
		friend std::ostream& operator<<(std::ostream& os, const SceneLoadGame& myClass);

		// Methods
		virtual bool		init();
		virtual void		load();
		virtual bool		update();

	protected:
		struct ButtonsStates {
			int64_t		selectedGame;
			bool		restart;
			bool		menu;
			bool		loadGame;
			bool		deleteGame;
		};
		ButtonsStates	_states;
		struct GameSaved {
			uint32_t		gameID;
			SettingsJson	*game;
			std::string		filename;
			ABaseUI			*ui;
		};
		std::vector<GameSaved *>	_gamesSaved;
		struct PreviewGame {
			ABaseUI *	title;
			ABaseUI *	date;
			ABaseUI *	gameDifficulty;
			ABaseUI *	levelsDone;
			ABaseUI *	scoreTotal;
			ABaseUI *	loadGame;
			ABaseUI *	deleteGame;
		};
		PreviewGame	previewGameUI;
		int64_t						_selectedGame;
		std::string					_lastSceneName;

	private:
		SceneLoadGame();
};
