#ifndef ASSIMPUTILS_HPP_
#define ASSIMPUTILS_HPP_

#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include "includesOpengl.hpp"

namespace AiUtils {
	// -- conversion from assimp to glm types ----------------------------------
	glm::mat4	aiToGlmMat(aiMatrix4x4 const &aiMat);
	glm::vec2	aiToVec2(aiVector3D const &aiVec2);
	glm::vec3	aiToVec3(aiVector3D const &aiVec3);
	glm::quat	aiToQuat(aiQuaternion const &aiQuat);
}

#endif  // ASSIMPUTILS_HPP_
