#pragma once

#include <chrono>

class AScene {
	public:
		AScene();
		AScene(AScene const & src);
		virtual ~AScene();

		virtual bool init() = 0;
		virtual bool update(std::chrono::milliseconds last_loop_ms) = 0;
		virtual bool draw() = 0;

		AScene & operator=(AScene const & rhs);
	protected:
	private:
};
