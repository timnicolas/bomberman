#include "EnemyFrog.hpp"
#include "Player.hpp"

// -- Constructors -------------------------------------------------------------

EnemyFrog::EnemyFrog(SceneGame &game)
: AEnemy(game),
  _dir(Direction::UP),
  _jumpGoal(VOID_POS),
  _nextJumpTime(0)
{
	name = "EnemyFrog";
	size = glm::vec3(0.7, 0.5, 0.7);
	strength = 0;  // remove auto damage
	resetCrossable();
}

EnemyFrog::~EnemyFrog() {
	auto it = game.enemies.begin();
	while (it != game.enemies.end()) {
		if ((*it) == this)
			game.enemies.erase(it);
		else
			it++;
	}
}

EnemyFrog::EnemyFrog(EnemyFrog const &src) : AEnemy(src) {
	*this = src;
}

// -- Operators ----------------------------------------------------------------

EnemyFrog &EnemyFrog::operator=(EnemyFrog const &rhs) {
	if ( this != &rhs ) {
		AEnemy::operator=(rhs);
	}
	return *this;
}

// -- Methods ------------------------------------------------------------------

/**
 * @brief Set the Entity that the Character can cross
 */
void EnemyFrog::resetCrossable() {
	ACharacter::resetCrossable();
	crossableTypes.clear();
	crossableTypes.push_back(Type::ALL);
}

/**
 * @brief update is called each frame.
 *
 * @param dTime Delta Time
 * @return true if success
 * @return false if failure
 */
bool	EnemyFrog::_update(float const dTime) {
	/* try jumping */
	if (_jumpGoal == VOID_POS && static_cast<uint64_t>(getMs().count()) >= _nextJumpTime) {
		// try to find a new destination
		_findJumpGoal();

		// If there is no possible destinations
		if (_jumpGoal == VOID_POS) {
			// always reset chrono (don't retry in the next update if there is no move options)
			_nextJumpTime = getMs().count() + WAIT_JUMP_TIME_MS;
		}
	}

	/* moving */
	// if jumping
	if (_jumpGoal != VOID_POS) {
		if (_isOn(_jumpGoal, dTime * speed * 3)) {  // end of the jump
			// set position with precision
			position.x = _jumpGoal.x;
			position.z = _jumpGoal.y;
			_jumpGoal = VOID_POS;

			// long or short wait
			if (rand() % CHANCE_LONG_WAIT == 0)
				_nextJumpTime = getMs().count() + LONG_WAIT_JUMP_TIME_MS;
			else
				_nextJumpTime = getMs().count() + WAIT_JUMP_TIME_MS;
		}
		_moveTo(_dir, dTime, -1);
	}
	else {  // if stay in a position
		if (game.player->hasCollision(position, size)) {
			game.player->takeDamage(1);
		}
	}

	return true;
}

/**
 * @brief Try to find a goal to jump on (This function can update _dir & _jumpGoal)
 */
void	EnemyFrog::_findJumpGoal() {
	int order = rand() % 3;  // random the order of elements (except go back option)
	Direction::Enum tryDirOrder[4] = {
		static_cast<Direction::Enum>((_dir + (order == 0 ? 0 : (order == 1 ? 1 : 3))) % 4),
		static_cast<Direction::Enum>((_dir + (order == 0 ? 3 : (order == 1 ? 1 : 0))) % 4),
		static_cast<Direction::Enum>((_dir + (order == 0 ? 1 : (order == 1 ? 3 : 0))) % 4),
		static_cast<Direction::Enum>((_dir + 2) % 4),
	};

	glm::ivec2 ipos = getIntPos();
	glm::ivec2 nextPos[4] = {
		{ 0, -2},  // UP
		{ 2,  0},  // RIGHT
		{ 0,  2},  // DOWN
		{-2,  0},  // LEFT
	};

	// try all directions
	for (int i = 0; i < 4; i++) {
		glm::ivec2 tmpPos(ipos.x + nextPos[tryDirOrder[i]].x, ipos.y + nextPos[tryDirOrder[i]].y);
		if (_canWalkOnBlock(tmpPos)) {
			// start a jump
			_dir = tryDirOrder[i];
			_jumpGoal = tmpPos;
			break;
		}
	}
}

/**
 * @brief postUpdate is called each frame. After update()
 *
 * @return true if success
 * @return false if failure
 */
bool	EnemyFrog::_postUpdate() {
	return true;
}

/**
 * @brief draw is called each frame.
 *
 * @return true if success
 * @return false if failure
 */
bool	EnemyFrog::_draw(Gui &gui) {
	gui.drawCube(Block::IA, getPos(), size);
	return true;
}
