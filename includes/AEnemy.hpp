#pragma once

#include <iostream>
#include <stdexcept>
#include <deque>
#include "ACharacter.hpp"
#include "SceneGame.hpp"

#define IS_ON_POS_OFFSET 0.2

#define INF_DIST 9999999

/**
 * @brief Path node element (to create auto path for enemy)
 */
struct PathNode {
	Direction::Enum	dir;  /**< Direction to move */
	glm::ivec2		goal;  /**< Goal to reach */
};

/**
 * @brief This is the base enemy object
 */
class AEnemy : public ACharacter {
private:
	AEnemy();

	/**
	 * @brief A node for DFS algorithm
	 */
	struct DFSNode {
		uint32_t	dist;  /**< Distance from start */
		glm::ivec2	last;  /**< Last position in path */

		DFSNode() : dist(INF_DIST), last{-1, -1} {}
	};
	void			_dfsExplore(std::vector<std::vector<DFSNode>> & boardDFS,
		glm::ivec2 pos, glm::ivec2 dest, bool & find);
	bool			_getPathToDFS(glm::ivec2 dest, std::deque<PathNode> & path);

protected:
	// Members
	Direction::Enum	_dir;  /**< The direction */
	std::vector<std::string>	_soundAttack;  /**< The sound on attacks */
	bool			_fisrtCall;  /**< true until the end of the first update call */
	bool			_moveOnCenter;  /**< true if the enemy move on center (size < 1) */

	/**
	 * @brief Update entity. Called on every frames
	 *
	 * @return false If failed
	 */
	virtual bool	_update() = 0;
	/**
	 * @brief Update entity. Called on every frames after update
	 *
	 * @return false If failed
	 */
	virtual bool	_postUpdate() = 0;
	/**
	 * @brief Draw entity. Called on every frames
	 *
	 * @param gui A reference to the gui object
	 * @return false If failed
	 */
	virtual bool	_draw(Gui &gui) = 0;

	/* movements functions */
	bool			_baseEnemyMove(Direction::Enum & dir);
	bool			_movePatternBasic(std::vector<Direction::Enum> directionOrder,
		uint32_t & dirIdx);
	bool			_followPath(std::deque<PathNode> & path);

	/* check and calculation functions */
	Direction::Enum	_isPlayerVisible() const;
	bool			_isOn(glm::ivec2 dest, float offset = IS_ON_POS_OFFSET) const;
	bool			_getPathTo(glm::ivec2 dest, std::deque<PathNode> & path);
	bool			_isBlocked();

public:
	// Members
	int			bombs;  /**< number of bombs */
	int32_t		points;  /**< number of points */
	int			strength;  /**< number of damage if hit player */

	// Constructors
	explicit AEnemy(SceneGame &game);
	~AEnemy();
	AEnemy(AEnemy const &src);

	// Operators
	AEnemy &operator=(AEnemy const &rhs);

	// Methods
	bool			update();
	bool			postUpdate();
	bool			draw(Gui &gui);
	virtual void	animEndCb(std::string animName);
	virtual bool	takeDamage(const int damage);
	std::unordered_set<AEntity *>	getCollision(glm::vec3 dest) const;

	// Exceptions
	/**
	 * @brief Enemy Exception
	 */
	class EnemyException : public std::runtime_error {
	public:
		EnemyException();
		explicit EnemyException(const char* whatArg);
	};
};
