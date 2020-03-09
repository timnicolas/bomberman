#ifndef GAME_HPP_
#define GAME_HPP_

#include <iostream>
#include <stdexcept>
#include <vector>
#include <map>
#include "useGlm.hpp"

#include "AScene.hpp"
#include "AEntity.hpp"
#include "ACharacter.hpp"
#include "Player.hpp"
#include "Bomb.hpp"

namespace GameState {
	enum Enum {
		PLAY,
		PAUSE,
		GAME_OVER,
	};
}

class SceneGame : public AScene {
private:
	SceneGame();
	// Members
	static std::map<std::string, AEntity *> _entitiesCall;

	// Methods
	bool	_loadLevel(uint8_t level);
	void	_drawBoard();

public:
	// Members
	std::vector< std::vector< std::vector<AEntity *> > > board;
	Player						*player;
	std::vector<ACharacter *>	enemies;
	std::vector<Bomb *>			bombs;

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

#endif  // GAME_HPP_
