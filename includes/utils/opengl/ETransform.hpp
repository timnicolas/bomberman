#ifndef ETRANSFORM_HPP_
#define ETRANSFORM_HPP_

#define WORLD_UP glm::vec3(0, 1, 0)

#include "useGlm.hpp"

class ETransform {
	public:
		ETransform(glm::vec3 pos = {0, 0, 0}, glm::vec3 scale = {0, 0, 0},
			glm::quat rotQ = glm::quat());
		ETransform(glm::vec3 pos = {0, 0, 0}, glm::vec3 scale = {0, 0, 0},
			glm::mat4 rotM = glm::rotate(glm::mat4(1.0), static_cast<float>(glm::radians(0.0)), WORLD_UP));
		ETransform(glm::vec3 pos = {0, 0, 0}, glm::vec3 scale = {0, 0, 0},
			float rotAngle = glm::radians(0.0), glm::vec3 rotAxis = WORLD_UP);

		virtual ~ETransform();
		ETransform(ETransform const &src);
		ETransform &operator=(ETransform const &rhs);

		glm::vec3	pos;
		glm::quat	rot;
		glm::vec3	scale;

	private:
		ETransform();
};

#endif  // ETRANSFORM_HPP_
