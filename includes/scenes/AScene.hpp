#pragma once

#include <chrono>

#include "Gui.hpp"

class AScene {
	public:
		explicit AScene(Gui * gui);
		AScene(AScene const & src);
		virtual ~AScene();

		virtual bool init() = 0;
		virtual bool update(std::chrono::milliseconds last_loop_ms) = 0;
		virtual bool draw() = 0;

		AScene & operator=(AScene const & rhs);

			// Exceptions
		class SceneException : public std::runtime_error {
		public:
			SceneException();
			explicit SceneException(const char* what_arg);
		};
	protected:
		AScene();

		Gui *	_gui;
};
