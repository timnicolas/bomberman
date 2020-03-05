#pragma once

#include "includesOpengl.hpp"
#include "ABaseUI.hpp"

#define SHADER_IMAGE_2D_VS "./shaders/image_2D_vs.glsl"
#define SHADER_IMAGE_2D_FS "./shaders/image_2D_fs.glsl"
#define SHADER_IMAGE_2D_ROW_SIZE 4

class ImageUI : public ABaseUI {
	public:
		ImageUI(glm::vec2 winSize, glm::vec2 pos, glm::vec2 size, std::string const & filename);
		ImageUI(ImageUI const & src);
		virtual ~ImageUI();

		ImageUI & operator=(ImageUI const & rhs);

		virtual void	draw();

	protected:
		virtual void	_update(glm::vec2 mousePos, bool rightClick, bool leftClick);
		void			_drawImg(glm::vec2 pos, glm::vec2 size, GLuint textureID, glm::vec4 color);
		ImageUI();

		std::string	_filename;
		Shader		_imgShader;
		GLuint		_vao;
		GLuint		_vbo;
		GLuint		_textureID;
};
