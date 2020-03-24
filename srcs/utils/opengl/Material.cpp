#include "Material.hpp"

Material::Material(glm::vec3 const diffuse, glm::vec3 const specular, \
glm::vec3 const ambient, float const shininess)
: diffuse(diffuse),
  specular(specular),
  ambient(ambient),
  shininess(shininess) {
}

Material::Material(Material const &src) {
	*this = src;
}

Material::~Material() {
}

Material &Material::operator=(Material const &rhs) {
	if (this != &rhs) {
		diffuse = rhs.diffuse;
		specular = rhs.specular;
		ambient = rhs.ambient;
		shininess = rhs.shininess;
	}
	return *this;
}


std::ostream & operator << (std::ostream &out, Material const &m) {
	out << "{" << std::endl;
	out << "  diffuse: " << glm::to_string(m.diffuse) << "," << std::endl;
	out << "  specular: " << glm::to_string(m.specular) << "," << std::endl;
	out << "  ambient: " << glm::to_string(m.ambient) << "," << std::endl;
	out << "  shininess: " << m.shininess << std::endl;
	out << "}" << std::endl;
	return out;
}
