#include <iomanip>
#include "Score.hpp"

// -- Constructors -------------------------------------------------------------

Score::Score() {
	_score = 0;
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
		_score = rhs._score;
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

int32_t	Score::getScore() const { return _score; }
Score	&Score::setScore(int32_t score) { _score = score; return *this; }

// -- Methods ------------------------------------------------------------------

Score	&Score::addPoints(int32_t points) {
	_score += points;
	return *this;
}

Score	&Score::addBonusTime(float const levelTime, float const time) {
	float	ratio = time / levelTime;
	if (ratio < 0.5)
		_score += 1000;
	else if (ratio < 0.75)
		_score += 600;
	return *this;
}

// -- Exceptions errors --------------------------------------------------------

Score::ScoreException::ScoreException()
: std::runtime_error("Score Exception") {}

Score::ScoreException::ScoreException(const char* what_arg)
: std::runtime_error(std::string(std::string("ScoreError: ") + what_arg).c_str()) {}
