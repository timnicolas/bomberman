#ifndef SCORE_HPP_
#define SCORE_HPP_

#include <iostream>
#include <stdexcept>
#include <vector>

class Score {
private:
	// Members
	int32_t		_score;  /**< Score */
	int32_t		_bonusDestruction;  /**< Bonus for blocks destruction */
	int32_t		_bonusEnemies;  /**< Bonus for enemy killed */
	int32_t		_bonusTime;  /**< Bonus for time left in level */
	int32_t		_levelId;  /**< Level ID */
	int32_t		_levelEnemies;  /**< Nb of eneny in level */
	int32_t		_killedEnemies;  /**< Nb of killed enemy in level */
	int32_t		_levelCrispies;  /**< Nb of crispy walls in level */
	int32_t		_crispiesDestroyed;  /**< Nb of crispy walls destroyed in level */
	float		_levelTime;  /**< Total time to do the level */
	float		_timeDone;  /**< Time for doing level */

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
	/**
	 * @brief Stats about score
	 */
	struct Stat {
		std::string		label;  /**< Label */
		std::string		points;  /**< Points */
		std::string		image;  /**< Img */
	};
	void	getStats(std::vector<Stat> &vec);

	// Exceptions
	/**
	 * @brief Score Exception
	 */
	class ScoreException : public std::runtime_error {
	public:
		ScoreException();
		explicit ScoreException(const char* what_arg);
	};
};

#endif  // SCORE_HPP_
