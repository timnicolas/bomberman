#version 410 core

#define NB_BLOCK_TYPES 22

layout (location = 0) in vec3 aPos;
layout (location = 1) in float aFaceId;

out GS_IN {
	flat int FaceId;
	vec3 Normal;
	flat int TextureId;
} vs_out;

uniform int[NB_BLOCK_TYPES * 6] blockTextures;
uniform sampler2DArray	textureAtlas;

uniform int blockId = 0;
uniform vec3 blockSize = vec3(1.0, 1.0, 1.0);

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform vec3 normals[6] = vec3[6](
	vec3(0.0f, 0.0f, 1.0f),
	vec3(1.0f, 0.0f, 0.0f),
	vec3(0.0f, 0.0f, -1.0f),
	vec3(-1.0f, 0.0f, 0.0f),
	vec3(0.0f, 1.0f, 0.0f),
	vec3(0.0f, -1.0f, 0.0f)
);

void main() {
	int	faceId = int(aFaceId);
	vs_out.FaceId = faceId;
	vs_out.Normal = mat3(transpose(inverse(model))) * normals[int(aFaceId)];

	// retrieve texture id
	vs_out.TextureId = blockTextures[blockId * 6 + int(aFaceId)];

	vec3 botLeftPos = aPos;
	if (faceId == 0) {
		botLeftPos.z += blockSize.z - 1.0;
	}
	else if (faceId == 1) {
		botLeftPos.x += blockSize.x - 1.0;
		botLeftPos.z += blockSize.z - 1.0;
	}
	else if (faceId == 2) {
		botLeftPos.x += blockSize.x - 1.0;
	}
	else if (faceId == 4) {
		botLeftPos.y += blockSize.y - 1.0;
		botLeftPos.z += blockSize.z - 1.0;
	}
	gl_Position = vec4(botLeftPos, 1.0);
}