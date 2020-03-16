#pragma once

#include "includesOpengl.hpp"
#include "ABaseUI.hpp"

/**
 * @brief this is the UI for images
 */
class ImageUI : public ABaseUI {
	public:
		ImageUI(glm::vec2 pos, glm::vec2 size, std::string const & filename);
		ImageUI(ImageUI const & src);
		virtual ~ImageUI();

		ImageUI & operator=(ImageUI const & rhs);

	protected:
		virtual void	_update();
		virtual void	_draw();
		ImageUI();
};
