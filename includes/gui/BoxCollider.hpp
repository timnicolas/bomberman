#pragma once

#include "useGlm.hpp"
#include "Gui.hpp"
#include "Shader.hpp"

#define BOX_SHADER_VS	"shaders/box_collider_vs.glsl"
#define BOX_SHADER_FS	"shaders/box_collider_fs.glsl"

class BoxCollider {
	public:
		BoxCollider();
		BoxCollider(BoxCollider const & src);
		virtual ~BoxCollider();

		BoxCollider & operator=(BoxCollider const & rhs);

		static bool				init(Gui * gui);
		static bool				destroy();
		static BoxCollider &	get();
		static bool				drawBox(glm::vec3 pos, glm::vec3 size, glm::vec4 color = {0, 0, 0, 1});

	protected:
		bool					_init(Gui * gui);
		bool					_destroy();
		bool					_drawBox(glm::vec3 pos, glm::vec3 size, glm::vec4 color);

		Gui *				_gui;
		glm::mat4			_projection;
		Shader *			_boxShader;
		uint32_t			_boxShaderVAO;
		uint32_t			_boxShaderVBO;
		static const float	_boxVertices[];
};
