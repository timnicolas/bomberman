#pragma once

#include "includesOpengl.hpp"
#include "ABaseUI.hpp"

class ImageUI : public ABaseUI {
	public:
		ImageUI(glm::vec2 pos, glm::vec2 size, std::string const & filename,
			bool pixelateOnZoom = true);
		ImageUI(ImageUI const & src);
		virtual ~ImageUI();

		ImageUI & operator=(ImageUI const & rhs);

		virtual void	draw();

	protected:
		virtual void	_update();
		ImageUI();
};
