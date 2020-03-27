#include <iomanip>
#include "Score.hpp"
#include "Logging.hpp"

// -- Constructors -------------------------------------------------------------

Score::Score() {
	_score = 0;
	_score = 0;
	_levelEnemies = 0;
	_killedEnemies = 0;
	_levelCrispies = 0;
	_crispiesDestroyed = 0;
	_levelTime = 0;
	_timeDone = 0;
	_levelId = -1;
}

Score::Score(int32_t score): _score(score) {
}

Score::~Score() {
}

Score::Score(Score const &src) {
	*this = src;
}

// -- Operators ----------------------------------------------------------------

Score &Score::operator=(Score const &rhs) {
	if ( this != &rhs ) {
		logWarn("Score object copied");
		_score = rhs._score;
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

Score	&Score::operator=(int32_t points) { return setScore(points); }
Score	&Score::operator+(int32_t points) { _score += points; return *this; }
Score	&Score::operator-(int32_t points) { _score -= points; return *this; }
void	Score::operator+=(int32_t points) { _score += points; }
void	Score::operator-=(int32_t points) { _score -= points; }

std::ostream &	operator<<(std::ostream & os, const Score& my_class) {
	os << std::setw(6) << std::setfill('0') << my_class.getScore();
	return (os);
}

// -- Accessors ----------------------------------------------------------------

/* _score */
int32_t	Score::getScore() const { return _score; }
Score	&Score::setScore(int32_t score) { _score = score; return *this; }
/* _levelId */
int32_t	Score::getLevelId() const { return _levelId; }
Score	&Score::setLevelId(int32_t levelId) { _levelId = levelId; return *this; }
/* toString */
std::string	Score::toString() {
	std::stringstream ss;
	ss << *this;
	std::string scoreStr = ss.str();
	return scoreStr;
}

// -- Methods ------------------------------------------------------------------

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
 * 		+ 10000 points if made in 50% of given time.
 * 		+ 600 points if made in 75% of given time.
 *
 * @param levelTime
 * @param time
 * @return Score&
 */
Score	&Score::addBonusTime(float const levelTime, float const time) {
	_levelTime = levelTime;
	_timeDone = time;
	float	ratio = time / levelTime;
	if (ratio < 0.5)
		_score += 10000;
	else if (ratio < 0.75)
		_score += 600;
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
	if (levelEnemies != 0) {
		if (_killedEnemies == 0) {
			if (levelCrispies != 0 && crispiesLast == 0) {
				// super bonus all crispies destroyed without kill any enemies
				_score += 20000;
			} else {
				// bonus no enemies killed.
				_score += 1000;
			}
		} else if (enemiesLast == 0){
			// bonus kill all enemies.
			_score += 500;
		}
	}
	return *this;
}

std::vector<std::string>	Score::getStats(std::vector<std::string> vec) {
	vec.push_back("Blocks destroyed: " + std::to_string(_crispiesDestroyed) + "/"+std::to_string(_levelCrispies));
	vec.push_back("Killed enemies: " + std::to_string(_killedEnemies) + "/"+std::to_string(_levelEnemies));
	std::string	timeDoneStr = std::to_string(_timeDone);
	timeDoneStr = timeDoneStr.substr(0, timeDoneStr.find(".")+2);
	std::string	levelTimeStr = std::to_string(_levelTime);
	levelTimeStr = levelTimeStr.substr(0, levelTimeStr.find(".")+2);
	vec.push_back("Time: " + timeDoneStr + "/"+levelTimeStr);
	return vec;
}

// -- Exceptions errors --------------------------------------------------------

Score::ScoreException::ScoreException()
: std::runtime_error("Score Exception") {}

Score::ScoreException::ScoreException(const char* what_arg)
: std::runtime_error(std::string(std::string("ScoreError: ") + what_arg).c_str()) {}
