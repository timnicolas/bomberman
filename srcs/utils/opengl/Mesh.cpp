#include "Mesh.hpp"
#include "Logging.hpp"
#include "debug.hpp"
#include "OpenGLModel.hpp"
#include "BoxCollider.hpp"

// -- Constructors -------------------------------------------------------------
/**
 * @brief Construct a new Mesh:: Mesh object
 *
 * @param openGLModel OpenGLModel master object
 * @param sh Shader
 * @param name Name
 * @param vertices Vertices
 * @param vertIndices Vertices indices
 * @param material Material
 * @param boundingBox Bounding box
 */
Mesh::Mesh(OpenGLModel &openGLModel, Shader &sh, std::string const &name,
	std::vector<Vertex> vertices, std::vector<uint32_t> vertIndices,
	Material material, BoundingBox boundingBox)
: _openGLModel(openGLModel),
  _sh(sh),
  _name(name),
  _vertices(vertices),
  _vertIndices(vertIndices),
  _textures({}),
  _material(material),
  _boundingBox(boundingBox),
  _vao(0),
  _vbo(0),
  _ebo(0) {}

/**
 * @brief Construct a new Mesh:: Mesh object
 *
 * @param src The object to do the copy
 */
Mesh::Mesh(Mesh const &src)
: _openGLModel(src._openGLModel),
  _sh(src._sh),
  _textures({})
{
	*this = src;
}

/**
 * @brief Destroy the Mesh:: Mesh object
 */
Mesh::~Mesh() {
	// free vao / vbo
	_sh.use();
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glDeleteBuffers(1, &_vbo);
	glDeleteBuffers(1, &_ebo);
	glDeleteVertexArrays(1, &_vao);
	_sh.unuse();
}

/**
 * @brief Copy this object
 *
 * @param rhs The object to copy
 * @return Mesh& A reference to the copied object
 */
Mesh &Mesh::operator=(Mesh const &rhs) {
	if (this != &rhs) {
		logWarn("Mesh is copied");

		_vertices = rhs._vertices;
		_vertIndices = rhs._vertIndices;
		_textures = rhs._textures;
		_material = rhs._material;
		_vao = rhs._vao;
		_vbo = rhs._vbo;
		_ebo = rhs._ebo;
	}
	return *this;
}

// -- setTexture ---------------------------------------------------------------------
/**
 * @brief set Mesh texture
 *
 * @param type the texture type DIFFUSE/SPECULAR/NORMAL/...
 * @param texture Texture object
 */
void	Mesh::setTexture(TextureType::Enum type, Texture const texture) {
	_textures[type] = texture;
}


// -- draw ---------------------------------------------------------------------
/**
 * @brief draw the Mesh
 *
 */
void	Mesh::draw(glm::mat4 const &model) const {
	// don't draw Mesh outside camera range
	if (!_openGLModel.isAnimated()) {
		glm::vec4 newCenter = model * glm::vec4(_boundingBox.center, 1.0);

		// show bounding box
		if (s.j("debug").j("show").b("boundingBox")) {
			glm::vec3 startPt = glm::vec3(newCenter) - _boundingBox.radius;
			glm::vec3 size = {_boundingBox.radius * 2, _boundingBox.radius * 2, _boundingBox.radius * 2};
			glm::vec4 color = colorise(s.j("colors").j("boundingBox").u("color"), s.j("colors").j("boundingBox").u("alpha"));
			BoxCollider::drawBox(startPt, size, color);
		}
		if (_openGLModel.getCam().frustumCullingCheckSphere(
			glm::vec3(newCenter), _boundingBox.radius) == FRCL_OUTSIDE) {
			return;
		}
	}

	// send textures
	_setUniformsTextures();

	// drawing mesh
	_sh.use();

	glBindVertexArray(_vao);
	glDrawElements(GL_TRIANGLES, _vertIndices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	_sh.unuse();
}

// -- setUniformsTextures ------------------------------------------------------
void	Mesh::_setUniformsTextures() const {
	bool	hasDiffuseTex = false;
	bool	hasSpecularTex = false;
	bool	hasNormalTex = false;

	_sh.use();
	/* set textures uniforms */
	uint32_t	i = 0;

	// diffuse texture
	auto	it = _textures.find(TextureType::DIFFUSE);
	if (it != _textures.end()) {
		glActiveTexture(GL_TEXTURE0 + i);
		hasDiffuseTex = true;
		_sh.setBool("material.diffuse.isTexture", true);
		_sh.setInt("material.diffuse.texture", i);
		glBindTexture(GL_TEXTURE_2D, it->second.id);
		++i;
	}

	// specular texture
	it = _textures.find(TextureType::SPECULAR);
	if (it != _textures.end()) {
		glActiveTexture(GL_TEXTURE0 + i);
		hasSpecularTex = true;
		_sh.setBool("material.specular.isTexture", true);
		_sh.setInt("material.specular.texture", i);
		glBindTexture(GL_TEXTURE_2D, it->second.id);
		++i;
	}

	// normal texture
	it = _textures.find(TextureType::NORMAL);
	if (it != _textures.end()) {
		glActiveTexture(GL_TEXTURE0 + i);
		hasNormalTex = true;
		_sh.setBool("material.normalMap.isTexture", true);
		_sh.setInt("material.normalMap.texture", i);
		glBindTexture(GL_TEXTURE_2D, it->second.id);
		++i;
	}

	glActiveTexture(GL_TEXTURE0);

	// set diffuse color if no texture has been setted
	if (!hasDiffuseTex) {
		_sh.setBool("material.diffuse.isTexture", false);
		_sh.setVec3("material.diffuse.color", _material.diffuse);
	}
	// set specular color if no texture has been setted
	if (!hasSpecularTex) {
		_sh.setBool("material.specular.isTexture", false);
		_sh.setVec3("material.specular.color", _material.specular);
	}
	// set normal color if no texture has been setted
	if (!hasNormalTex) {
		_sh.setBool("material.normalMap.isTexture", false);
		_sh.setVec3("material.normalMap.color", glm::vec3(0, 0, 1));
	}
	_sh.setFloat("material.shininess", _material.shininess);

	_sh.unuse();
}

// -- sendMesh -----------------------------------------------------------------
/**
 * @brief send the mesh data to openGL
 *
 */
void	Mesh::sendMesh() {
	// create vao, vbo, ebo
	glGenVertexArrays(1, &_vao);
	glGenBuffers(1, &_vbo);
	glGenBuffers(1, &_ebo);

	// fill vao buffer
	glBindVertexArray(_vao);
	glBindBuffer(GL_ARRAY_BUFFER, _vbo);
	glBufferData(GL_ARRAY_BUFFER, _vertices.size() * sizeof(Vertex),
		&_vertices[0], GL_STATIC_DRAW);

	// set-up ebo
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, _vertIndices.size() * sizeof(u_int32_t),
		&_vertIndices[0], GL_STATIC_DRAW);

	// vertex positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
		reinterpret_cast<void *>(0));
	// vertex normals
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
		reinterpret_cast<void *>(offsetof(Vertex, norm)));
	// vertex texture coords
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
		reinterpret_cast<void *>(offsetof(Vertex, texCoords)));
	// vertex tangent
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
		reinterpret_cast<void *>(offsetof(Vertex, tangents)));
	// vertex bones Id
	glEnableVertexAttribArray(4);
	glVertexAttribIPointer(4, NB_BONES_PER_VERTEX, GL_INT, sizeof(Vertex),
		reinterpret_cast<void *>(offsetof(Vertex, bonesId)));
	// vertex bones weight
	glEnableVertexAttribArray(5);
	glVertexAttribPointer(5, NB_BONES_PER_VERTEX, GL_FLOAT, GL_FALSE, sizeof(Vertex),
		reinterpret_cast<void *>(offsetof(Vertex, bonesW)));

    glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

// -- addBoneData --------------------------------------------------------------
/**
 * @brief store the bone info to the mesh
 *
 * @param boneID the bone id
 * @param weight the weight of the bone to the vertex
 * @param vertexID the vertex id
 */
void Mesh::addBoneData(u_int32_t boneID, float weight, u_int32_t vertexID) {
	for (u_int32_t i = 0; i < NB_BONES_PER_VERTEX; ++i) {
		// if the vertex bone weight as not been set
		if (_vertices[vertexID].bonesW[i] == 0.0f) {
			_vertices[vertexID].bonesId[i] = boneID;
			_vertices[vertexID].bonesW[i] = weight;
			return;
		}
	}
	logWarn("Mesh: " << _name << ", too many bones for vertex: " << vertexID <<
		", max: " << NB_BONES_PER_VERTEX);
}

// -- Vertex -------------------------------------------------------------------
Vertex::Vertex() {
	// init all bones to 0
	bonesId = {{0}};
	bonesW = {{0.0f}};
}
