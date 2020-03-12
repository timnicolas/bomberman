#ifndef SCENEGAME_HPP_
#define SCENEGAME_HPP_

#include <iostream>
#include <stdexcept>
#include <vector>
#include <map>
#include "useGlm.hpp"

#include "AScene.hpp"
#include "AEntity.hpp"
#include "ACharacter.hpp"
#include "Bomb.hpp"

class Player;

namespace GameState {
	enum Enum {
		PLAY,
		PAUSE,
		GAME_OVER,
	};
}

namespace EntityType {
	enum Enum {
		PLAYER,
		BOARD,
		BOARD_FLAG,
		ENEMY,
	};
}

class SceneGame : public AScene {
private:
	SceneGame();
	// Members
	typedef AEntity*(*entityFuncPtr)(SceneGame &);
	struct Entity {
		EntityType::Enum	entityType;
		entityFuncPtr		entity;
	};
	static std::map<std::string, Entity> _entitiesCall;

	// Methods
	bool	_loadLevel(uint8_t levelId);
	bool	_initJsonLevel(SettingsJson &lvl, uint8_t levelId);
	void	_drawBoard();

public:
	// Members
	std::vector< std::vector< std::vector<AEntity *> > > board;
	Player						*player;
	std::vector<ACharacter *>	enemies;

	int							flags;
	glm::uvec2					size;
	uint8_t						level;
	GameState::Enum				state;
	std::chrono::milliseconds	time;

	// Constructors
	explicit SceneGame(Gui * gui, float const &dtTime);
	virtual ~SceneGame();
	SceneGame(SceneGame const &src);

	// Operators
	SceneGame &operator=(SceneGame const &rhs);
	friend std::ostream& operator<<(std::ostream& os, const SceneGame& myClass);

	// Methods
	std::string		print() const;
	bool			clearFromBoard(AEntity *entity, glm::vec2 pos);
	bool			positionInGame(glm::vec2 pos);

	// AScene methods
	virtual bool	init();
	virtual bool	update();
	virtual bool	postUpdate();
	virtual bool	draw();
	virtual void	load();
	virtual void	unload();
};

#endif  // SCENEGAME_HPP_
