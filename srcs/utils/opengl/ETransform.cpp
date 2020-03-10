#include "ETransform.hpp"

ETransform::ETransform(glm::vec3 pos, glm::vec3 scale, glm::quat rotQ)
: pos(pos),
  rot(rotQ),
  scale(scale) {}

ETransform::ETransform(glm::vec3 pos, glm::vec3 scale, glm::mat4 rotM)
: ETransform(pos, scale, glm::toQuat(rotM)) {}

ETransform::ETransform(glm::vec3 pos, glm::vec3 scale, float rotAngle,
glm::vec3 rotAxis)
: ETransform(pos, scale, glm::rotate(glm::mat4(1.0), rotAngle, rotAxis)) {}

ETransform::~ETransform() {
}

ETransform::ETransform(ETransform const &src) {
	*this = src;
}

ETransform &ETransform::operator=(ETransform const &rhs) {
	if (this != &rhs) {
		pos = rhs.pos;
		rot = rhs.rot;
		scale = rhs.scale;
	}

	return *this;
}
