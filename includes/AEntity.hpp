#ifndef ENTITY_HPP_
#define ENTITY_HPP_

#include <iostream>
#include <stdexcept>
#include "useGlm.hpp"
#include "Logging.hpp"
#include "Gui.hpp"
#include "ModelsManager.hpp"

class Model;
class SceneGame;

namespace Category {
	/**
	 * @brief This is the entity category.
	 *
	 * Entity can be static (wall, bonus, ...) or mobile (enemy, player, ...)
	 */
	enum Enum {
		STATIC,
		MOBILE,
	};
}  // namespace Category
namespace Type {
	/**
	 * @brief Entity type
	 */
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
		SPAWNER,
		ALL,
		NONE,
	};
}  // namespace Type

namespace EntityState {
	/**
	 * @brief Entity state (used for animation: AEntity::setState)
	 */
	enum Enum {
		IDLE,
		DYING,
		ATTACK,
		RUNNING,
		DROP_BOMB,
		LOOSE_EMOTE,
		VICTORY_EMOTE,
		TRANSFORM_IN,
		TRANSFORM_OUT,
	};

	/**
	 * @brief Used to save the state and a bool to know if the state has been updated
	 */
	struct	Struct {
		EntityState::Enum	state;  /**< The entity state */
		bool				updated;  /**< True if the state was updated */
	};
}  // namespace EntityState

/**
 * @brief This is the base class for entity (Charactere, Objects, ...)
 */
class AEntity {
private:
	AEntity();

protected:
	float				_timeToDie;  /**< The time to die (animation) */
	EntityState::Struct	_entityState;  /**< The entity state (current animation: run, idle, ...) */
	Model				*_model;  /**< The 3D model of the entity */
	bool				_animDeathEnd;  /**< True if the entity has a death animation */
	glm::vec3			size;  /**< The entity size */

public:
	// Members
	bool			active;  /**< True if the entity is active */
	bool			alive;  /**< True if the entity is alive */
	Category::Enum	category;  /**< The entity category (Category::STATIC | Category::MOBILE) */
	std::string		name;  /**< The entity name */
	Type::Enum		type;  /**< The entity type */
	SceneGame		&game;  /**< A reference to the main scene for game */
	bool			destructible;  /**< True if the entity is destructible (by bombs) */
	bool			blockPropagation;  /**< Used in bombs for explosion propagation */
	glm::vec3		position;  /**< The entity position */

	// Constructors
	explicit AEntity(SceneGame &game);
	virtual ~AEntity();
	AEntity(const AEntity &src);

	// Operators
	AEntity			&operator=(const AEntity &rhs);

	// Methods
	virtual bool		init();
	/**
	 * @brief Update entity. Called on every frames
	 *
	 * @return false If failed
	 */
	virtual bool		update() = 0;
	virtual bool		postUpdate();
	/**
	 * @brief Draw entity. Called on every frames
	 *
	 * @param gui A reference to the gui object
	 * @return false If failed
	 */
	virtual bool		draw(Gui &gui) = 0;
	virtual bool		drawCollider();
	/**
	 * @brief Get the Pos object
	 *
	 * @return glm::vec3 The position
	 */
	virtual glm::vec3	getPos() const = 0;
	/**
	 * @brief Call this function to take damage
	 *
	 * @param damage Number of damage taken
	 * @return false If not active or indestructible
	 */
	virtual bool		takeDamage(const int damage) = 0;
	/**
	 * @brief Get the Board object
	 *
	 * @return std::vector< std::vector< std::vector<AEntity *> > > const& The board object
	 */
	virtual std::vector< std::vector< std::vector<AEntity *> > > const &	getBoard() const = 0;
	/**
	 * @brief Get the Board object
	 *
	 * @return std::vector< std::vector< std::vector<AEntity *> > > const& The board object
	 */
	virtual std::vector< std::vector< std::vector<AEntity *> > > &			getBoard() = 0;
	virtual void		animEndCb(std::string animName);
	void				setState(EntityState::Enum state);
	EntityState::Enum	getState() const;
};

#endif  // ENTITY_HPP_
