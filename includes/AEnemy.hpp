#pragma once

#include <iostream>
#include <stdexcept>
#include <deque>
#include "ACharacter.hpp"
#include "SceneGame.hpp"

#define IS_ON_POS_OFFSET 0.2

#define INF_DIST 9999999

struct PathNode {
	Direction::Enum	dir;
	glm::ivec2		goal;
};

/**
 * @brief This is the base enemy object
 */
class AEnemy : public ACharacter {
private:
	AEnemy();

	struct DFSNode {
		uint32_t	dist;
		glm::ivec2	last;

		DFSNode() : dist(INF_DIST), last{-1, -1} {}
	};
	void			_dfsExplore(std::vector<std::vector<DFSNode>> & boardDFS, glm::ivec2 pos, glm::ivec2 dest, bool & find);
	bool			_getPathToDFS(glm::ivec2 dest, std::deque<PathNode> & path);

protected:
	virtual bool	_update(float const dTime) = 0;
	virtual bool	_postUpdate() = 0;
	virtual bool	_draw(Gui &gui) = 0;

	/* movements functions */
	bool			_movePatternBasic(float const dTime, std::vector<Direction::Enum> directionOrder, uint32_t & dirIdx);
	bool			_followPath(float const dTime, std::deque<PathNode> & path);

	/* check and calcultation functions */
	Direction::Enum	_isPlayerVisible() const;
	bool			_isOn(glm::ivec2 dest) const;
	bool			_getPathTo(glm::ivec2 dest, std::deque<PathNode> & path);

public:
	// Members
	int			bombs;

	// Constructors
	explicit AEnemy(SceneGame &game);
	~AEnemy();
	AEnemy(AEnemy const &src);

	// Operators
	AEnemy &operator=(AEnemy const &rhs);

	// Methods
	bool			update(float const dTime);
	bool			postUpdate();
	bool			draw(Gui &gui);

	// Exceptions
	class EnemyException : public std::runtime_error {
	public:
		EnemyException();
		explicit EnemyException(const char* whatArg);
	};
};
