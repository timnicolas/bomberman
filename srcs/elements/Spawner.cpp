#include "Spawner.hpp"
#include "SceneGame.hpp"
#include "Player.hpp"
#include "Bonus.hpp"

#include "EnemyBasic.hpp"
#include "EnemyFollow.hpp"
#include "EnemyWithEye.hpp"
#include "EnemyFly.hpp"
#include "EnemyCrispy.hpp"
#include "EnemyFrog.hpp"

// -- Constructors -------------------------------------------------------------

/**
 * @brief Construct a new Spawner:: Spawner object
 *
 * @param game A reference to the SceneGame master object
 */
Spawner::Spawner(SceneGame &game) : AObject(game) {
	type = Type::SPAWNER;
	name = "Spawner";
	destructible = false;
	blockPropagation = false;
	// _typeEnemy = std::vector<std::string>();
	_frequency = 0;
	_waitForSpawn = 0;
}

/**
 * @brief Destroy the Spawner:: Spawner object
 */
Spawner::~Spawner() {
}

/**
 * @brief Construct a new Spawner:: Spawner object
 *
 * @param src The object to do the copy
 */
Spawner::Spawner(Spawner const &src) : AObject(src) {
	*this = src;
}

// -- Operators ----------------------------------------------------------------

/**
 * @brief Copy this object
 *
 * @param rhs The object to copy
 * @return Spawner& A reference to the copied object
 */
Spawner &Spawner::operator=(Spawner const &rhs) {
	if ( this != &rhs ) {
		AObject::operator=(rhs);
		_typeEnemy = rhs._typeEnemy;
		_frequency = rhs._frequency;
		_waitForSpawn = rhs._waitForSpawn;
	}
	return *this;
}

// -- Getter & setters ---------------------------------------------------------

/**
 * @brief Set the enemy type
 *
 * @param typeEnemy The enemy type
 * @return Spawner& A ref to the spawner object
 */
Spawner &Spawner::setTypeEnemy(std::vector<std::string> typeEnemy) {
	_typeEnemy.clear();
	_typeEnemy = typeEnemy;
	return *this;
}

/**
 * @brief Add new enemy type
 *
 * @param typeEnemy The enemy type
 * @return Spawner& A ref to the spawner object
 */
Spawner &Spawner::addTypeEnemy(std::string typeEnemy) {
	std::vector<std::string>::iterator it = std::find(_typeEnemy.begin(), _typeEnemy.end(), typeEnemy);
	if (it == _typeEnemy.end()) {
		_typeEnemy.push_back(typeEnemy);
	}
	return *this;
}

/**
 * @brief Set the spawning frequency
 *
 * @param frequency The frequency
 * @return Spawner& A ref to the spawner object
 */
Spawner &Spawner::setFrequency(int64_t frequency) {
	_frequency = frequency;
	return *this;
}

// -- Methods ------------------------------------------------------------------

/**
 * @brief update is called each frame.
 *
 * @return true if success
 * @return false if failure
 */
bool	Spawner::update() {
	if (!active)
		return true;
	if (!alive) {
		active = false;
		return true;
	}
	if (_frequency == 0)
		return true;

	_waitForSpawn -= game.getDtTime();
	if (_waitForSpawn < 0) {
		int enemyKind = rand() % _typeEnemy.size();
		bool fly = _typeEnemy[enemyKind] == ENEMY_FLY_STR ? true : false;
		if (game.insertEntity(_typeEnemy[enemyKind], {position.x, position.z}, fly, 0)) {
			game.levelEnemies++;
			// reset for new spawn
			_waitForSpawn = _frequency;
		} else {
			// wait a little more time to spawn
			_waitForSpawn = 1;
		}
	}

	return true;
}

/**
 * @brief Called after update
 *
 * @return false If failed
 */
bool	Spawner::postUpdate() {
	return true;
}

/**
 * @brief draw is called each frame.
 *
 * @return true if success
 * @return false if failure
 */
bool	Spawner::draw(Gui &gui) {
	(void)gui;
	return true;
}


// -- Exceptions errors --------------------------------------------------------

/**
 * @brief Construct a new Spawner:: Spawner Exception:: Spawner Exception object
 */
Spawner::SpawnerException::SpawnerException()
: std::runtime_error("Spawner Exception") {}

/**
 * @brief Construct a new Spawner:: Spawner Exception:: Spawner Exception object
 *
 * @param whatArg Error message
 */
Spawner::SpawnerException::SpawnerException(const char* whatArg)
: std::runtime_error(std::string(std::string("SpawnerError: ") + whatArg).c_str()) {}
