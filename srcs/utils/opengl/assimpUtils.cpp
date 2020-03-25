#include <glm/gtc/type_ptr.hpp>  // for make_mat4

#include "assimpUtils.hpp"

// -- conversion from assimp to glm types --------------------------------------
glm::mat4	AiUtils::aiToGlmMat(aiMatrix4x4 const &aiMat) {
	return glm::transpose(glm::make_mat4(&aiMat.a1));
}

glm::vec2	AiUtils::aiToVec2(aiVector3D const &aiVec2) {
	return glm::vec2(aiVec2.x, aiVec2.y);
}

glm::vec3	AiUtils::aiToVec3(aiVector3D const &aiVec3) {
	return glm::vec3(aiVec3.x, aiVec3.y, aiVec3.z);
}

glm::quat	AiUtils::aiToQuat(aiQuaternion const &aiQuat) {
	return glm::quat(aiQuat.w, aiQuat.x, aiQuat.y, aiQuat.z);
}
