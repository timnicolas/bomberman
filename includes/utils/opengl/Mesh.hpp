#ifndef MESH_HPP_
#define MESH_HPP_

#define NB_BONES_PER_VERTEX 4

#include <vector>
#include <unordered_map>
#include <array>

#include "includesOpengl.hpp"
#include "Shader.hpp"
#include "Material.hpp"

class OpenGLModel;

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

struct	BoundingBox {
	glm::vec3	startPoint;
	glm::vec3	size;
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
		Mesh(OpenGLModel &openGLModel, Shader &sh, std::string const &name,
			std::vector<Vertex> vertices, std::vector<uint32_t> vertIndices,
			Material material, BoundingBox boundingBox);
		virtual ~Mesh();
		Mesh(Mesh const &src);
		Mesh &operator=(Mesh const &rhs);

		void	setTexture(Texture const texture);
		void	draw(glm::mat4 const &model) const;
		void	addBoneData(uint32_t boneID, float weight, uint32_t VerticeID);
		void	sendMesh();

	private:
		Mesh();  // private default constructor, should not be called
		void	_setUniformsTextures() const;

		OpenGLModel				&_openGLModel;
		Shader					&_sh;
		std::string				_name;
		std::vector<Vertex>		_vertices;
		std::vector<uint32_t>	_vertIndices;  // contain _vertices id
		std::unordered_map<TextureType::Enum, Texture>	_textures;
		Material				_material;
		BoundingBox				_boundingBox;
        uint32_t				_vao;
        uint32_t				_vbo;
        uint32_t				_ebo;
};

#endif  // MESH_HPP_
