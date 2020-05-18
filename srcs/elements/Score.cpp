#include <iomanip>
#include "Score.hpp"
#include "Logging.hpp"
#include "bomberman.hpp"

// -- Constructors -------------------------------------------------------------

/**
 * @brief Construct a new Score:: Score object
 */
Score::Score() {
	reset();
}

/**
 * @brief Construct a new Score:: Score object
 *
 * @param score A starting score
 */
Score::Score(int32_t score): _score(score) {
}

/**
 * @brief Destroy the Score:: Score object
 */
Score::~Score() {
}

/**
 * @brief Construct a new Score:: Score object
 *
 * @param src The object to do the copy
 */
Score::Score(Score const &src) {
	*this = src;
}

// -- Operators ----------------------------------------------------------------

/**
 * @brief Copy this object
 *
 * @param rhs The opjet to copy
 * @return Score& A reference to the copied object
 */
Score &Score::operator=(Score const &rhs) {
	if ( this != &rhs ) {
		logWarn("Score object copied");
		_score = rhs._score;
		_bonusDestruction = rhs._bonusDestruction;
		_bonusEnemies = rhs._bonusEnemies;
		_bonusTime = rhs._bonusTime;
		_levelEnemies = rhs._levelEnemies;
		_killedEnemies = rhs._killedEnemies;
		_levelCrispies = rhs._levelCrispies;
		_crispiesDestroyed = rhs._crispiesDestroyed;
		_levelTime = rhs._levelTime;
		_timeDone = rhs._timeDone;
		_levelId = rhs._levelId;
	}
	return *this;
}

/**
 * @brief Create a score with a given points
 *
 * @param points The points
 * @return Score& The score created
 */
Score	&Score::operator=(int32_t points) { return setScore(points); }
/**
 * @brief Add to score
 *
 * @param points Points to add
 * @return Score& A reference to the score object
 */
Score	&Score::operator+(int32_t points) { _score += points; return *this; }
/**
 * @brief Sub to score
 *
 * @param points Points to add
 * @return Score& A reference to the score object
 */
Score	&Score::operator-(int32_t points) { _score -= points; return *this; }
/**
 * @brief Add to score
 *
 * @param points Points to add
 * @return Score& A reference to the score object
 */
void	Score::operator+=(int32_t points) { _score += points; }
/**
 * @brief Sub to score
 *
 * @param points Points to add
 * @return Score& A reference to the score object
 */
void	Score::operator-=(int32_t points) { _score -= points; }

/**
 * @brief Display score object
 *
 * @param os The ostream object
 * @param my_class A reference to 'this' object
 * @return std::ostream& A reference to the ostream object
 */
std::ostream &	operator<<(std::ostream & os, const Score& my_class) {
	os << std::setw(6) << std::setfill('0') << my_class.getScore();
	return (os);
}

// -- Accessors ----------------------------------------------------------------

/* _score */
/**
 * @brief Get the score
 *
 * @return int32_t The score
 */
int32_t	Score::getScore() const { return _score + _bonusDestruction + _bonusEnemies + _bonusTime; }
/**
 * @brief Set the score
 *
 * @param score The new score
 * @return Score& A reference to 'this' object
 */
Score	&Score::setScore(int32_t score) { _score = score; return *this; }
/* _levelId */
/**
 * @brief Get the level ID
 *
 * @return int32_t Current level ID
 */
int32_t	Score::getLevelId() const { return _levelId; }
/**
 * @brief Set the level ID
 *
 * @param levelId New level ID
 * @return Score& A reference to 'this' object
 */
Score	&Score::setLevelId(int32_t levelId) { _levelId = levelId; return *this; }
/* toString */
/**
 * @brief Convert object to string
 *
 * @return std::string String version of object
 */
std::string	Score::toString() {
	std::stringstream ss;
	ss << *this;
	std::string scoreStr = ss.str();
	return scoreStr;
}

// -- Methods ------------------------------------------------------------------

/**
 * @brief Reset Score.
 *
 * @return Score& this
 */
Score	&Score::reset() {
	_score = 0;
	_bonusDestruction = 0;
	_bonusEnemies = 0;
	_bonusTime = 0;
	_levelEnemies = 0;
	_killedEnemies = 0;
	_levelCrispies = 0;
	_crispiesDestroyed = 0;
	_levelTime = 0;
	_timeDone = 0;
	_levelId = -1;
	return *this;
}

/**
 * @brief Add points to scrore.
 *
 * @param points
 * @return Score& this
 */
Score	&Score::addPoints(int32_t points) {
	_score += points;
	return *this;
}

/**
 * @brief Add bonus points for time.
 *
 * @param levelTime
 * @param time
 * @return Score&
 */
Score	&Score::addBonusTime(float const levelTime, float const time) {
	_levelTime = levelTime;
	_timeDone = time;
	_bonusTime = levelTime - time;
	float	ratio = time / levelTime;
	if (ratio < 0.25)
		_bonusTime *= 100;
	else if (ratio < 0.75)
		_bonusTime *= 25;
	return *this;
}

/**
 * @brief Add bonus points according to number of killed enemies.
 * 		+ 20000 points if destroyed every crispies and no enemy killed
 * 		+ 1000 points if no enemy killed
 * 		+ 500 points if all enemies are dead.
 *
 * @param levelEnemies
 * @param enemiesLast
 * @param levelCrispies
 * @param crispiesLast
 * @return Score&
 */
Score	&Score::addBonusEnemies(uint32_t levelEnemies, uint32_t enemiesLast,
uint32_t levelCrispies, uint32_t crispiesLast)
{
	_levelEnemies = levelEnemies;
	_killedEnemies = levelEnemies - enemiesLast;
	_levelCrispies = levelCrispies;
	_crispiesDestroyed = levelCrispies - crispiesLast;
	if (levelCrispies != 0 && crispiesLast == 0) {
		_bonusDestruction = 125;
	}
	if (levelEnemies != 0) {
		if (_killedEnemies == 0) {
			// bonus no enemies killed.
			_bonusEnemies = 1000;
			if (levelCrispies != 0 && crispiesLast == 0) {
				// super bonus all crispies destroyed without kill any enemies
				_bonusDestruction = 19000;
			}
		} else if (enemiesLast == 0){
			// bonus kill all enemies.
			_bonusEnemies = 500;
		}
	}
	return *this;
}

/**
 * @brief Get Statistic Score
 *
 * @param vec
 * @return std::vector<Score::Stat>
 */
void	Score::getStats(std::vector<Score::Stat> &vec) {
	vec.push_back({
		"Blocks destroyed:",
		std::to_string(_crispiesDestroyed) + "/"+std::to_string(_levelCrispies),
		""
	});
	vec.push_back({
		"Bonus destruction: ",
		std::to_string(_bonusDestruction) + " pts",
		""
	});
	vec.push_back({
		"Killed enemies: " + std::to_string(_killedEnemies) + "/"+std::to_string(_levelEnemies),
		std::to_string(_score) + " pts",
		""
	});
	vec.push_back({
		"Bonus enemies: ",
		std::to_string(_bonusEnemies) + " pts",
		""
	});
	vec.push_back({
		"Time: " + timeToString(_timeDone) + " / " + timeToString(_levelTime),
		std::to_string(_bonusTime) + " pts",
		"bomberman-assets/textures/bonus/time.png"
	});
	vec.push_back({
		"TOTAL",
		std::to_string(getScore()) + " pts",
		getScore() > 15000
			? "bomberman-assets/textures/bonus/score_active.png"
			: "bomberman-assets/textures/bonus/score.png"
	});
}

// -- Exceptions errors --------------------------------------------------------

/**
 * @brief Construct a new Score:: Score Exception:: Score Exception object
 */
Score::ScoreException::ScoreException()
: std::runtime_error("Score Exception") {}

/**
 * @brief Construct a new Score:: Score Exception:: Score Exception object
 *
 * @param what_arg Error message
 */
Score::ScoreException::ScoreException(const char* what_arg)
: std::runtime_error(std::string(std::string("ScoreError: ") + what_arg).c_str()) {}
