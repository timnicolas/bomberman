#ifndef MESH_HPP_
#define MESH_HPP_

#define NB_BONES_PER_VERTEX 4

#include <vector>
#include <map>
#include <array>

#include "includesOpengl.hpp"
#include "Shader.hpp"
#include "Material.hpp"

namespace TextureType {
	enum Enum {
		DIFFUSE,
		SPECULAR,
		NORMAL
	};
}

/**
 * @brief Struct to store vertex infos
 *
 */
struct	Vertex {
	glm::vec3	pos;
	glm::vec3	norm;
	glm::vec2	texCoords;
	glm::vec3	tangents;
	std::array<int, NB_BONES_PER_VERTEX>	bonesId;
	std::array<float, NB_BONES_PER_VERTEX>	bonesW;

	Vertex();  // default constructor, init bonesId and bonesW
};

struct	Texture {
	uint32_t			id;
	TextureType::Enum	type;
	std::string			path;
};

/**
 * @brief Class to store and draw a 3d model Mesh
 *
 */
class	Mesh {
	public:
		Mesh(Shader &sh, std::string const &name, std::vector<Vertex> vertices,
			std::vector<uint32_t> vertIndices, std::vector<Texture> textures,
			Material material);
		virtual ~Mesh();
		Mesh(Mesh const &src);
		Mesh &operator=(Mesh const &rhs);

		void	draw() const;
		void	addBoneData(uint32_t boneID, float weight, uint32_t VerticeID);
		void	sendMesh();

	private:
		Mesh();  // private default constructor, should not be called
		void	_setUniformsTextures() const;

		Shader					&_sh;
		std::string				_name;
		std::vector<Vertex>		_vertices;
		std::vector<uint32_t>	_vertIndices;  // contain _vertices id
		std::vector<Texture>	_textures;
		Material				_material;
        uint32_t				_vao;
        uint32_t				_vbo;
        uint32_t				_ebo;
};

#endif  // MESH_HPP_
