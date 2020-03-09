#ifndef SCENEGAME_HPP_
#define SCENEGAME_HPP_

#include <iostream>
#include <stdexcept>
#include <vector>
#include <map>
#include <glm/glm.hpp>
#include "glm/gtx/string_cast.hpp"

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
		BOMB,
	};
}

class SceneGame : public AScene {
private:
	SceneGame();
	// Members
	typedef AEntity*(*entityFuncPtr)(SceneGame &);
	struct Entity {
		EntityType::Enum	entity_type;
		entityFuncPtr		entity;
	};
	static std::map<std::string, Entity> _entitiesCall;

	// Methods
	bool	_loadLevel(uint8_t level_id);
	bool	_initJsonLevel(SettingsJson &lvl, uint8_t level_id);
	void	_drawBoard();

public:
	// Members
	std::vector< std::vector< std::vector<AEntity *> > > board;
	Player						*player;
	std::vector<ACharacter *>	enemies;
	std::vector<Bomb *>			bombs;

	uint8_t						flags;
	glm::uvec2					size;
	uint8_t						level;
	GameState::Enum				state;
	std::chrono::milliseconds	time;

	// Constructors
	explicit SceneGame(Gui * gui);
	virtual ~SceneGame();
	SceneGame(SceneGame const &src);

	// Operators
	SceneGame &operator=(SceneGame const &rhs);
	friend std::ostream& operator<<(std::ostream& os, const SceneGame& my_class);

	// Methods
	std::string		print() const;

	// AScene methods
	virtual bool	init();
	virtual bool	update(std::chrono::milliseconds last_loop_ms);
	virtual bool	draw();
};

#endif  // SCENEGAME_HPP_
