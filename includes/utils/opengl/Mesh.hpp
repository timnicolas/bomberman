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
	glm::vec3	pos;  /**< The position */
	glm::vec3	norm;  /**< The normal */
	glm::vec2	texCoords;  /**< The texture coordinate */
	glm::vec3	tangents;  /**< The tangents */
	std::array<int, NB_BONES_PER_VERTEX>	bonesId;  /**< The IDs of the related bones */
	std::array<float, NB_BONES_PER_VERTEX>	bonesW;  /**< The weight of the related bones */

	Vertex();  // default constructor, init bonesId and bonesW
};

/**
 * @brief Bounding box
 */
struct	BoundingBox {
	glm::vec3	center;  /**< The center */
	float		radius;  /**< The radius */
};

/**
 * @brief Texture
 */
struct	Texture {
	uint32_t			id;  /**< The texture ID */
	TextureType::Enum	type;  /**< The texture type (DIFFUSE, SPECULAR, NORMAL) */
	std::string			path;  /**< The texture path */
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

		void	setTexture(TextureType::Enum type, Texture const texture);
		void	draw(glm::mat4 const &model) const;
		void	addBoneData(uint32_t boneID, float weight, uint32_t vertexID);
		void	sendMesh();

	private:
		Mesh();  // private default constructor, should not be called
		void	_setUniformsTextures() const;

		OpenGLModel				&_openGLModel;  /**< A reference to the model */
		Shader					&_sh;  /**< A reference to the shader */
		std::string				_name;  /**< The mesh name */
		std::vector<Vertex>		_vertices;  /**< All mesh vertices */
		std::vector<uint32_t>	_vertIndices;  /**< Contains _vertices id */
		std::unordered_map<TextureType::Enum, Texture>	_textures;  /**< Contains all textures */
		Material				_material;  /**< The material */
		BoundingBox				_boundingBox;  /**< The boundingBox */
        uint32_t				_vao;  /**< Vertex Array Objects */
        uint32_t				_vbo;  /**< Vertex Buffer Objects */
        uint32_t				_ebo;  /**< Element Buffer Objects */
};

#endif  // MESH_HPP_
