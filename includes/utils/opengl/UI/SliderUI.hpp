#pragma once

#include "includesOpengl.hpp"
#include "ABaseUI.hpp"

/**
 * @brief this is the UI for slider
 */
class SliderUI : public ABaseUI {
	public:
		SliderUI(glm::vec2 pos, glm::vec2 size);
		SliderUI(SliderUI const & src);
		virtual ~SliderUI();

		SliderUI & operator=(SliderUI const & rhs);

		void			setValues(float min, float max, float val, float step = 0);
		void			setValue(float val);

		SliderUI &		addSliderListener(float * listener);

	protected:
		virtual void	_update();
		virtual void	_draw();
		SliderUI();

		float	_min;  /**< Minimum value */
		float	_max;  /**< Maximum value */
		float	_val;  /**< Actual value */
		float	_step;  /**< Step btw 2 values */
		float *	_sliderListener;  /**< slider element value */
};
