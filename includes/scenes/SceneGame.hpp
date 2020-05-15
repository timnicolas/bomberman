#ifndef SCENEGAME_HPP_
#define SCENEGAME_HPP_

#include <iostream>
#include <stdexcept>
#include <vector>
#include <map>
#include "useGlm.hpp"

#include "ASceneMenu.hpp"
#include "AEntity.hpp"
#include "ACharacter.hpp"
#include "Bomb.hpp"
#include "Score.hpp"
#include "Spawner.hpp"

#include "ABaseUI.hpp"
#include "TextUI.hpp"
#include "Model.hpp"

#define NO_LEVEL -1  // value is no level loaded
#define LEVEL_INTRO_DURATION 2

class Player;
class AEnemy;
class Spawner;

namespace GameState {
	/**
	 * @brief All possible game states (INTRO, PLAY, PAUSE, GAME_OVER, ...)
	 */
	enum Enum {
		INTRO,
		PLAY,
		PAUSE,
		GAME_OVER,
		WIN,
	};
}  // namespace GameState

namespace EntityType {
	/**
	 * @brief All possible entity states
	 */
	enum Enum {
		PLAYER,
		BOARD,
		BOARD_FLAG,
		ENEMY,
	};
}  // namespace EntityType

/**
 * @brief This is the game Scene. In this scene, you can play to the game and load levels
 */
class SceneGame : public ASceneMenu {
private:
	SceneGame();
	// Members
	typedef AEntity*(*entityFuncPtr)(SceneGame &);
	/**
	 * @brief Contains a type of entity and a function to create it
	 */
	struct Entity {
		EntityType::Enum	entityType;  /**< The type of the entity */
		entityFuncPtr		entity;  /**< The function to create the entity */
	};

	std::vector<SettingsJson *>	_mapsList;  /**<  */

	/**
	 * @brief All 3D models of enemies that can be drawed on differents menu
	 */
	struct DrawForMenu {
		Model *	player;  /**< 3D model to draw player */
		Model *	flower;  /**< 3D model to draw flower */
		Model *	robot;  /**< 3D model to draw robot */
		Model * fly;  /**< 3D model to draw fly */
		Model * frog;  /**< 3D model to draw frog */
		Model * crispy;  /**< 3D model to draw crispy */
		Model * follow;  /**< 3D model to draw follow */

		DrawForMenu();
	};
	DrawForMenu _menuModels;  /**< All 3D elements to draw */
	bool	_alarm;  /**< If we want to ring alarm */
	Model	*_terrain;  /**< The terrain element */

	// Methods
	bool	_loadLevel(int32_t levelId);
	bool	_unloadLevel();
	bool	_initJsonLevel(int32_t levelId);
	void	_drawBoard();

protected:
	/**
	 * @brief All UI elements (bonus, text, ...)
	 */
	struct AllUI {
		ABaseUI *	introText;  /**< TextUI for introText */
		ABaseUI *	timeLeftImg;  /**< ImageUI for timeLeftImg */
		ABaseUI *	timeLeftImgActive;  /**< ImageUI for timeLeftImgActive */
		ABaseUI *	timeLeftText;  /**< TextUI for timeLeftText */
		ABaseUI *	scoreImg;  /**< ImageUI for scoreImg */
		ABaseUI *	scoreImgActive;  /**< ImageUI for scoreImgActive */
		ABaseUI *	scoreText;  /**< TextUI for scoreText */
		ABaseUI *	lifeImg;  /**< ImageUI for lifeImg */
		ABaseUI *	lifeImgActive;  /**< ImageUI for lifeImgActive */
		ABaseUI *	lifeText;  /**< TextUI for lifeText */
		ABaseUI *	levelNameText;  /**< TextUI for levelNameText */
		ABaseUI *	enemiesCounterText;  /**< TextUI for enemiesCounterText */
		ABaseUI *	speedImg;  /**< ImageUI for speedImg */
		ABaseUI *	speedImgActive;  /**< ImageUI for speedImgActive */
		ABaseUI *	speedText;  /**< TextUI for speedText */
		ABaseUI *	bonusBombImg;  /**< ImageUI for bonusBombImg */
		ABaseUI *	bonusBombImgActive;  /**< ImageUI for bonusBombImgActive */
		ABaseUI *	bonusBombText;  /**< TextUI for bonusBombText */
		ABaseUI *	bonusFlameImg;  /**< ImageUI for bonusFlameImg */
		ABaseUI *	bonusFlameImgActive;  /**< ImageUI for bonusFlameImgActive */
		ABaseUI *	bonusFlameText;  /**< TextUI for bonusFlameText */
		ABaseUI *	bonusFlampassImg;  /**< ImageUI for bonusFlampassImg */
		ABaseUI *	bonusFlampassImgActive;  /**< ImageUI for bonusFlampassImgActive */
		ABaseUI *	bonusWallpassImg;  /**< ImageUI for bonusWallpassImg */
		ABaseUI *	bonusWallpassImgActive;  /**< ImageUI for bonusWallpassImgActive */
		ABaseUI *	bonusDetonatorImg;  /**< ImageUI for bonusDetonatorImg */
		ABaseUI *	bonusDetonatorImgActive;  /**< ImageUI for bonusDetonatorImgActive */
		ABaseUI *	bonusBombpassImg;  /**< ImageUI for bonusBombpassImg */
		ABaseUI *	bonusBombpassImgActive;  /**< ImageUI for bonusBombpassImgActive */
		ABaseUI *	bonusShieldImg;  /**< ImageUI for bonusShieldImg */
		ABaseUI *	bonusShieldImgActive;  /**< ImageUI for bonusShieldImgActive */
		ABaseUI *	bonusShieldText;  /**< TextUI for bonusShieldText */
	};
	AllUI		allUI;  /**< All UI elements */

	void			_initGameInfos();
	void			_loadGameInfos();
	void			_updateGameInfos();
	bool			_initBonus();

	std::vector<CamPoint>	_getIntroAnim() const;
	std::vector<CamPoint>	_getGameOverAnim() const;
	std::vector<CamPoint>	_getVictoryAnim() const;

public:
	// Members
	static std::map<std::string, Entity>	entitiesCall;  /**< All entity type & functions */
	std::vector< std::vector< std::vector<AEntity *> > > board;  /**< The base board with all static elements */
	std::vector< std::vector< std::vector<AEntity *> > > boardFly;  /**< The fly board with all static flying elements */
	Player						*player;  /**< The player */
	std::vector<AEnemy *>		enemies;  /**< All enemies */
	/**
	 * @brief Bonus Information about spawn (nomber of bonus & chance to spawn)
	 */
	struct BonusValues {
		int64_t	chance;  /**< Chance to have a bonus */
		int64_t	nb;  /**< Number of bonus on the level */
	};
	std::unordered_map<std::string, BonusValues>	bonus;  /**< All bonus information about spawn */
	std::vector<Spawner *>		spawners;  /**< All spawners */

	int							flags;  /**< Number of flags on the level */
	glm::uvec2					size;  /**< Level size */
	int32_t						level;  /**< The current level ID (-1 for no level) */
	GameState::Enum				state;  /**< Actual game state (PLAY, PAUSE, GAME_OVER, ...) */
	uint32_t					levelEnemies;  /**< Number of enemies in the level */
	uint32_t					levelCrispies;  /**< Number of cripy wall in the level */
	float						levelTime;  /**< Time to do the level */
	float						time;  /**< Time remaining to do the level */
	Score						score;  /**< Score object */
	int64_t						enemiesToKill;  /**< Enemy to kill to enable end element & finish the level */
	int64_t						enemiesKilled;  /**< Number of enemies killed */
	std::string					musicLevel;  /**< The level music */

	// Constructors
	SceneGame(Gui * gui, float const &dtTime);
	virtual ~SceneGame();
	SceneGame(SceneGame const &src);

	// Operators
	SceneGame &operator=(SceneGame const &rhs);
	friend std::ostream& operator<<(std::ostream& os, const SceneGame& myClass);

	// Methods
	std::string		print() const;
	bool			clearFromBoard(AEntity *entity, glm::vec2 pos);
	bool			positionInGame(glm::vec3 pos, glm::vec3 sz = glm::vec3(1, 1, 1));

	// SceneGame methods
	virtual bool	init();
	virtual bool	update();
	virtual bool	postUpdate();
	virtual void	load();
	virtual void	unload();
	virtual bool	draw();
	bool			updateForMenu();
	bool			drawGame();
	bool			drawForMenu();
	bool			drawVictory();
	bool			drawGameOver();
	bool			drawEndGame();
	bool			loadLevel(int32_t levelId);
	bool			insertEntity(std::string const & name, glm::ivec2 pos, bool isFly = false, uint64_t wallGenPercent = 0);

	// getter
	uint32_t		getNbLevel() const;
	std::string		getLevelName(int32_t levelId) const;
	std::string		getLevelImg(int32_t levelId) const;
	SettingsJson	&getSettingsLevel() const;
	static std::vector<std::string>	getAllEntityNames();

	// Exceptions
	class SceneGameException : public std::runtime_error {
	public:
		SceneGameException();
		/**
		 * @brief Construct a new Scene Game Exception object
		 *
		 * @param whatArg Error message
		 */
		explicit SceneGameException(const char* whatArg);
	};
};

#endif  // SCENEGAME_HPP_
