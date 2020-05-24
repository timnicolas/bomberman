#ifndef SCENEDEBUG_HPP_
#define SCENEDEBUG_HPP_

#define DEBUG_FONT			"cheatcode"
#define DEBUG_FONT_SCALE	1
#define DEBUG_TEXT_COLOR	glm::vec4(.978, .97, .967, 1)

#define UPDATE_DEBUG_DATA_MS	300

#include <chrono>

#include "ASceneMenu.hpp"
#include "TextInputUI.hpp"

/**
 * @brief debug menu to show fps at the top of the screen
 */
class SceneDebug : public ASceneMenu {
	public:
		// Constructors
		SceneDebug(Gui *gui, float const &dtTime);
		virtual ~SceneDebug();
		SceneDebug(SceneDebug const &src);
		SceneDebug &operator=(SceneDebug const &rhs);

		// Methods
		virtual bool		init();
		virtual bool		update();

		// Setters
		void	setVisible(bool visible = true);

	private:
		SceneDebug();

		std::chrono::milliseconds	_lastUpdateMs;  /**< Last time fps was updated */
		bool		_visible;  /**< If FPs are visible or invisible */
		uint16_t	_fps;  /**< Actual FPS */
		TextUI		*_fpsText;  /**< FPS text element */
		TextUI		*_nbentitiesText;  /**< Number of entities text element */
		TextUI		*_nbenemiesText;  /**< Number of enemies text element */
};

#endif  // SCENEDEBUG_HPP_
