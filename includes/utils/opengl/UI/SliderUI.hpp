#pragma once

#include "includesOpengl.hpp"
#include "ABaseUI.hpp"

class SliderUI : public ABaseUI {
	public:
		SliderUI(glm::vec2 pos, glm::vec2 size);
		SliderUI(SliderUI const & src);
		virtual ~SliderUI();

		SliderUI & operator=(SliderUI const & rhs);

		virtual void	_draw();

		void			setValues(float min, float max, float val, float step = 0);

		SliderUI &		addSliderListener(float * listener);

	protected:
		virtual void	_update(glm::vec2 mousePos, bool rightClick, bool leftClick);
		SliderUI();

		float	_min;
		float	_max;
		float	_val;
		float	_step;
		float *	_sliderListener;
};
