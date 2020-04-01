#include <stack>
#include "AEnemy.hpp"
#include "Player.hpp"

/**
 * @brief Get the path to a destination
 *
 * @param dest The destination
 * @param path The path (empty if there is no path).
 * @return true if a path is detected
 */
bool AEnemy::_getPathTo(glm::ivec2 dest, std::deque<PathNode> & path) {
	return _getPathToDFS(dest, path);
}

/* DFS IMPLEMENTATION ------------------------------------------ */

/**
 * @brief Get the path to a destination with DFS algorithm
 *
 * DFS has been modified to always find the shortest path
 *
 * @param dest The destination
 * @param path The path (empty if there is no path).
 * @return true if a path is detected
 */
bool AEnemy::_getPathToDFS(glm::ivec2 dest, std::deque<PathNode> & path) {
	std::vector<std::vector<DFSNode>> boardDFS;
	boardDFS.resize(game.size.x);
	for (auto && boardLine : boardDFS) {
		boardLine.resize(game.size.y);
	}
	bool find = false;
	_dfsExplore(boardDFS, getIntPos(), dest, find);
	if (find) {
		path.clear();
		DFSNode * tmp = &boardDFS[dest.x][dest.y];
		glm::ivec2 curPos = dest;
		// create the final path
		while (tmp->last.x >= 0) {
			// create new path element
			PathNode elem;
			elem.goal = curPos;
			if (curPos.x < tmp->last.x)			elem.dir = Direction::LEFT;
			else if (curPos.x > tmp->last.x)	elem.dir = Direction::RIGHT;
			else if (curPos.y < tmp->last.y)	elem.dir = Direction::UP;
			else if (curPos.y > tmp->last.y)	elem.dir = Direction::DOWN;
			path.push_front(elem);

			// do to the next element
			curPos = tmp->last;
			tmp = &boardDFS[tmp->last.x][tmp->last.y];
		}
	}
	return find;
}
void AEnemy::_dfsExplore(std::vector<std::vector<DFSNode>> & boardDFS, glm::ivec2 pos, glm::ivec2 dest, bool & find) {
	if (pos == dest)
		find = true;
	if (boardDFS[pos.x][pos.y].last.x < 0) {
		boardDFS[pos.x][pos.y].dist = 0;
	}
	int nexts[4][2] = {
		{-1,  0},
		{ 1,  0},
		{ 0, -1},
		{ 0,  1},
	};
	for (int i = 0; i < 4; i++) {
		glm::ivec2 tmpPos(pos.x + nexts[i][0], pos.y + nexts[i][1]);
		if (_canWalkOnBlock(tmpPos) && boardDFS[tmpPos.x][tmpPos.y].dist > boardDFS[pos.x][pos.y].dist + 1) {
			boardDFS[tmpPos.x][tmpPos.y].last = pos;
			boardDFS[tmpPos.x][tmpPos.y].dist = boardDFS[pos.x][pos.y].dist + 1;
			_dfsExplore(boardDFS, tmpPos, dest, find);
		}
	}
}
