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
	out << "material_____________" << std::endl;
	out << " diffuse: (" << m.diffuse.x << ", " << m.diffuse.y << ", " << m.diffuse.z << ")" << std::endl;
	out << " specular: (" << m.specular.x << ", " << m.specular.y << ", " << m.specular.z << ")" << std::endl;
	out << " ambient: (" << m.ambient.x << ", " << m.ambient.y << ", " << m.ambient.z << ")" << std::endl;
	out << " shininess: " << m.shininess << std::endl;
	return out;
}
