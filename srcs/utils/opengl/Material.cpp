#include "Material.hpp"

/**
 * @brief Construct a new Material:: Material object
 *
 * @param name Name
 * @param diffuse Diffuse
 * @param specular Specular
 * @param ambient Ambient
 * @param shininess Shininess
 */
Material::Material(std::string const name, glm::vec3 const diffuse,
	glm::vec3 const specular, glm::vec3 const ambient, float const shininess)
: name(name),
  diffuse(diffuse),
  specular(specular),
  ambient(ambient),
  shininess(shininess) {
}

/**
 * @brief Construct a new Material:: Material object
 *
 * @param src The object to do the copy
 */
Material::Material(Material const &src) {
	*this = src;
}

/**
 * @brief Destroy the Material:: Material object
 */
Material::~Material() {
}

/**
 * @brief Copy this object
 *
 * @param rhs The object to copy
 * @return Material& A reference to the copied object
 */
Material &Material::operator=(Material const &rhs) {
	if (this != &rhs) {
		name = rhs.name;
		diffuse = rhs.diffuse;
		specular = rhs.specular;
		ambient = rhs.ambient;
		shininess = rhs.shininess;
	}
	return *this;
}


std::ostream & operator << (std::ostream &out, Material const &m) {
	out << m.name << ": {" << std::endl;
	out << "  diffuse: " << glm::to_string(m.diffuse) << "," << std::endl;
	out << "  specular: " << glm::to_string(m.specular) << "," << std::endl;
	out << "  ambient: " << glm::to_string(m.ambient) << "," << std::endl;
	out << "  shininess: " << m.shininess << std::endl;
	out << "}" << std::endl;
	return out;
}
