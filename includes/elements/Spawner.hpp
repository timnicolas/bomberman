#ifndef SPAWNER_HPP_
#define SPAWNER_HPP_

#include <iostream>
#include <stdexcept>
#include <vector>
#include "useGlm.hpp"
#include "AObject.hpp"
#include "AEnemy.hpp"

class Spawner : public AObject {
private:
	std::vector<std::string>	_typeEnemy;
	int64_t						_frequency;
	float						_waitForSpawn;

	Spawner();

public:
	// Constructors
	explicit Spawner(SceneGame &game);
	~Spawner();
	Spawner(Spawner const &src);

	// Operators
	Spawner &operator=(Spawner const &rhs);

	// Getters & setters
	Spawner &setTypeEnemy(std::vector<std::string> typeEnemy);
	Spawner &addTypeEnemy(std::string typeEnemy);
	Spawner &setFrequency(int64_t frequency);

	// Methods
	bool	update();
	bool	postUpdate();
	bool	draw(Gui &gui);

	// Exceptions
	class SpawnerException : public std::runtime_error {
	public:
		SpawnerException();
		explicit SpawnerException(const char* whatArg);
	};
};

#endif  // SPAWNER_HPP_
