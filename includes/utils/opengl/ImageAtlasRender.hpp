#pragma once

#include "includesOpengl.hpp"
#include "TextureManager.hpp"
#include "Shader.hpp"

#define SHADER_IMAGE_VS "./shaders/image_atlas_vs.glsl"
#define SHADER_IMAGE_FS "./shaders/image_atlas_fs.glsl"
#define SHADER_IMAGE_ROW_SIZE 5

/**
 * @brief Render an image atlas (load png & draw image by ID)
 */
class ImageAtlasRender {
	public:
		ImageAtlasRender(TextureManager const &textureManager, uint32_t width, \
		uint32_t height);
		ImageAtlasRender(ImageAtlasRender const &src);
		virtual ~ImageAtlasRender();

		ImageAtlasRender &operator=(ImageAtlasRender const &rhs);
		void		draw(glm::vec2 pos, glm::vec2 size, int texID, \
						glm::vec4 color = glm::vec4(1, 1, 1, 1));
		void		draw(float posx, float posy, float w, float h, int texID, \
						glm::vec4 color = glm::vec4(1, 1, 1, 1));

		Shader			&getShader();
		Shader const	&getShader() const;

		class ImageRenderError : public std::exception {
			public:
				virtual const char* what() const throw() = 0;
		};
		class LoadImageException : public ImageRenderError {
			public:
				virtual const char* what() const throw() {
					return ("Fail to load image");
				}
		};

	private:
		ImageAtlasRender();

		TextureManager const	&_textureManager;  /**< A ref to the TextureManager object */
		Shader		_shader;  /**< Shader */
		glm::mat4	_projection;  /**< Projection matrix */
		GLuint		_vao;  /**< Vertex Array Objects */
		GLuint		_vbo;  /**< Vertex buffer Objects */
};
