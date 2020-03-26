#ifndef SCORE_HPP_
#define SCORE_HPP_

#include <iostream>
#include <stdexcept>

class Score {
private:
	// Members
	int32_t		_score;

public:
	// Constructors
	Score();
	explicit Score(int32_t score);
	~Score();
	Score(Score const &src);

	// Operators
	Score	&operator=(Score const &rhs);
	Score	&operator=(int32_t points);
	Score	&operator+(int32_t points);
	Score	&operator-(int32_t points);
	void	operator+=(int32_t points);
	void	operator-=(int32_t points);
	friend std::ostream& operator<<(std::ostream& os, const Score& my_class);

	// Accessors
	int32_t	getScore() const;
	Score	&setScore(int32_t score);


	// Methods
	Score	&addPoints(int32_t points);
	Score	&addBonusTime(float const levelTime, float const time);

	// Exceptions
	class ScoreException : public std::runtime_error {
	public:
		ScoreException();
		explicit ScoreException(const char* what_arg);
	};
};

#endif  // SCORE_HPP_
