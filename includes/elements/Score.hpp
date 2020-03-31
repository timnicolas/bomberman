#ifndef SCORE_HPP_
#define SCORE_HPP_

#include <iostream>
#include <stdexcept>
#include <vector>

class Score {
private:
	// Members
	int32_t		_score;
	int32_t		_bonusDestruction;
	int32_t		_bonusEnemies;
	int32_t		_bonusTime;
	int32_t		_levelId;
	int32_t		_levelEnemies;
	int32_t		_killedEnemies;
	int32_t		_levelCrispies;
	int32_t		_crispiesDestroyed;
	float		_levelTime;
	float		_timeDone;

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
	int32_t	getLevelId() const;
	Score	&setLevelId(int32_t levelId);
	std::string	toString();

	// Methods
	Score	&reset();
	Score	&addPoints(int32_t points);
	Score	&addBonusTime(float const levelTime, float const time);
	Score	&addBonusEnemies(uint32_t levelEnemies, uint32_t enemiesLast,
			uint32_t levelCrispies, uint32_t crispiesLast);
	struct Stat {
		std::string		label;
		std::string		points;
		std::string		image;
	};
	void	getStats(std::vector<Stat> &vec);

	// Exceptions
	class ScoreException : public std::runtime_error {
	public:
		ScoreException();
		explicit ScoreException(const char* what_arg);
	};
};

#endif  // SCORE_HPP_
