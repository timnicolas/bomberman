#pragma once

#include <vector>

#include "includesOpengl.hpp"
#include "Shader.hpp"

#define SHADER_SKYBOX_VS "shaders/skybox_vs.glsl"
#define SHADER_SKYBOX_FS "shaders/skybox_fs.glsl"

#define SKYBOX_USING 0
#if SKYBOX_USING == 0
	#define SKYBOX_START	"./bomberman-assets/skybox/bomberSky/"
	#define SKYBOX_EXT		".jpg"
	#define SKYBOX_NAME_TYPE 2
#endif

#if SKYBOX_NAME_TYPE == 0
	#define SKYBOX_NAME_RIGHT	"right"
	#define SKYBOX_NAME_LEFT	"left"
	#define SKYBOX_NAME_TOP		"top"
	#define SKYBOX_NAME_BOTTOM	"bottom"
	#define SKYBOX_NAME_FRONT	"front"
	#define SKYBOX_NAME_BACK	"back"
#elif SKYBOX_NAME_TYPE == 1
	#define SKYBOX_NAME_RIGHT	"ft"
	#define SKYBOX_NAME_LEFT	"bk"
	#define SKYBOX_NAME_TOP		"up"
	#define SKYBOX_NAME_BOTTOM	"dn"
	#define SKYBOX_NAME_FRONT	"rt"
	#define SKYBOX_NAME_BACK	"lf"
#else  // http://www.custommapmakers.org/skyboxes.php#
	#define SKYBOX_NAME_LEFT	"nx"
	#define SKYBOX_NAME_RIGHT	"px"
	#define SKYBOX_NAME_TOP		"py"
	#define SKYBOX_NAME_BOTTOM	"ny"
	#define SKYBOX_NAME_FRONT	"pz"
	#define SKYBOX_NAME_BACK	"nz"
#endif

/**
 * @brief create a skybox for a project
 */
class Skybox {
	public:
		Skybox();
		Skybox(Skybox const &src);
		virtual ~Skybox();

		Skybox &operator=(Skybox const &rhs);

		void load(std::vector<std::string> &faces);
		void draw(float nightProgress = 0);

		Shader			&getShader();
		Shader const	&getShader() const;
		uint32_t		getTextureID() const;
	protected:
	private:
		Shader		_shader;  /**< Skybox shader */
		uint32_t	_textureID;  /**< Skybox texture ID */
		uint32_t	_vao;  /**< Vertex Array Objects */
		uint32_t	_vbo;  /**< Vertex Buffer Objects */

		static const float _vertices[];
};
