#ifndef ENTITY_HPP_
#define ENTITY_HPP_

#include <iostream>
#include <stdexcept>
#include "useGlm.hpp"
#include "Logging.hpp"
#include "Gui.hpp"
class SceneGame;

namespace Category {
	enum Enum {
		STATIC,
		MOBILE,
	};
}
namespace Type {
	enum Enum {
		PLAYER,
		ENEMY,
		BOMB,
		FIRE,
		WALL,
		CRISPY,
		FLAG,
		END,
		BONUS,
		ALL,
		NONE,
	};
}  // namespace Type

namespace EntityState {
	enum Enum {
		IDLE,
		DYING,
		RUNNING,
		DROP_BOMB,
		LOOSE_EMOTE,
		VICTORY_EMOTE,
	};

	struct	Struct {
		EntityState::Enum	state;
		bool				updated;
	};
}  // namespace EntityState

/**
 * @brief This is the base class for entity (Charactere, Objects, ...)
 */
class AEntity {
private:
	AEntity();

protected:
	float				_timeToDie;
	EntityState::Struct	_entityState;

public:
	// Members
	bool			active;
	bool			alive;
	Category::Enum	category;
	std::string		name;
	Type::Enum		type;
	SceneGame		&game;
	bool			destructible;
	bool			blockPropagation;
	glm::vec3		position;

	// Constructors
	explicit AEntity(SceneGame &game);
	virtual ~AEntity();
	AEntity(const AEntity &src);

	// Operators
	AEntity			&operator=(const AEntity &rhs);

	// Methods
	virtual bool		update() = 0;
	virtual bool		postUpdate();
	virtual bool		draw(Gui &gui) = 0;
	virtual glm::vec3	getPos() = 0;
	virtual bool		takeDamage(const int damage) = 0;
	virtual std::vector< std::vector< std::vector<AEntity *> > > const &	getBoard() const = 0;
	virtual std::vector< std::vector< std::vector<AEntity *> > > &			getBoard() = 0;
	virtual void	animEndCb(std::string animName);
	void			setstate(EntityState::Enum state);
};

#endif  // ENTITY_HPP_
