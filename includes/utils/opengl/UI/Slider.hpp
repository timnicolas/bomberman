#pragma once

#include "includesOpengl.hpp"
#include "ABaseUI.hpp"

class Slider : public ABaseUI {
	public:
		Slider(glm::vec2 winSize, glm::vec2 pos, glm::vec2 size);
		Slider(Slider const & src);
		virtual ~Slider();

		Slider & operator=(Slider const & rhs);

		virtual void	draw();

		void			setValues(float min, float max, float val, float step = 0);

		Slider &		addSliderListener(float * listener);

	protected:
		virtual void	_update(glm::vec2 mousePos, bool rightClick, bool leftClick);
		Slider();

		float	_min;
		float	_max;
		float	_val;
		float	_step;
		float *	_sliderListener;
};
