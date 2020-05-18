#pragma once

#include <chrono>

#include "Gui.hpp"

/**
 * @brief base object to implement in all scenes
 */
class AScene {
	public:
		AScene(Gui * gui, float const &dtTime);
		AScene(AScene const & src);
		virtual ~AScene();

		/**
		 * @brief Init the scene
		 *
		 * @return false If failed
		 */
		virtual bool init() = 0;
		/**
		 * @brief Update the scene (called every frames)
		 *
		 * @return false If failed
		 */
		virtual bool update() = 0;
		/**
		 * @brief Draw the scene (called evey frames)
		 *
		 * @return false If failed
		 */
		virtual bool draw() = 0;
		/**
		 * @brief Load the scene (called on every scene loading)
		 */
		virtual void load() = 0;
		/**
		 * @brief Unload the scene (called on every scene unloading)
		 */
		virtual void unload() = 0;

		float const	&getDtTime() const;

		AScene & operator=(AScene const & rhs);

		// Exceptions
		/**
		 * @brief Scene exception
		 */
		class SceneException : public std::runtime_error {
		public:
			SceneException();
			explicit SceneException(const char* whatArg);
		};

	protected:
		AScene();

		Gui *	_gui;  /**< A reference to the gui object */
		float const &_dtTime;  /**< A reference to the delta time */
};
