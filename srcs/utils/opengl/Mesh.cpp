#include "Mesh.hpp"
#include "Logging.hpp"
#include "debug.hpp"

// -- Constructors -------------------------------------------------------------
Mesh::Mesh(Shader &sh, std::string const &name, std::vector<Vertex> vertices,
	std::vector<u_int32_t> vertIndices, std::vector<Texture> textures,
	Material material)
: _sh(sh),
  _name(name),
  _vertices(vertices),
  _vertIndices(vertIndices),
  _textures(textures),
  _material(material),
  _vao(0),
  _vbo(0),
  _ebo(0) {}

Mesh::Mesh(Mesh const &src) : _sh(src._sh) {
	*this = src;
}

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

Mesh &Mesh::operator=(Mesh const &rhs) {
	if (this != &rhs) {
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

// -- draw ---------------------------------------------------------------------
void	Mesh::draw() const {
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
	// set textures uniforms
	uint32_t	i = 0;
	for (Texture const &tex : _textures) {
		glActiveTexture(GL_TEXTURE0 + i);

		// diffuse texture
		if (tex.type == TextureType::DIFFUSE && !hasDiffuseTex) {
			hasDiffuseTex = true;
			_sh.setBool("material.diffuse.isTexture", true);
			_sh.setInt("material.diffuse.texture", i);
			glBindTexture(GL_TEXTURE_2D, tex.id);
		}
		// specular texture
		else if (tex.type == TextureType::SPECULAR && !hasSpecularTex) {
			hasSpecularTex = true;
			_sh.setBool("material.specular.isTexture", true);
			_sh.setInt("material.specular.texture", i);
			glBindTexture(GL_TEXTURE_2D, tex.id);
		}
		// normal texture
		else if (tex.type == TextureType::NORMAL && !hasNormalTex) {
			hasNormalTex = true;
			_sh.setBool("material.normalMap.isTexture", true);
			_sh.setInt("material.normalMap.texture", i);
			glBindTexture(GL_TEXTURE_2D, tex.id);
		}
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
		// TODO(zer0nim): need to set other default normal ?
		_sh.setVec3("material.normalMap.color", glm::vec3(0, 0, 1));
	}
	_sh.setFloat("material.shininess", _material.shininess);

	_sh.unuse();
}

// -- sendMesh -----------------------------------------------------------------
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
	bonesId = {0};
	bonesW = {0.0f};
}
