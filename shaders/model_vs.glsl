#version 410 core

#define MAX_BONES 100
#define NUM_BONES_PER_VERTEX 4

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in ivec4 bonesID;
layout (location = 5) in vec4 bonesWeight;

out VS_OUT {
	vec2 TexCoords;
	vec3 TangentLightDir;
	vec3 TangentViewPos;
	vec3 TangentFragPos;
	vec3 TangentNormal;
} vs_out;

uniform bool isAnimated;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 modelScale;
uniform mat4 bones[MAX_BONES];

struct DirLight {
	vec3		direction;

	vec3		ambient;
	vec3		diffuse;
	vec3		specular;
};

uniform vec3 viewPos;
uniform DirLight dirLight;

void main() {
	mat4 boneTransform = mat4(0.0);
	for (int i=0; i < NUM_BONES_PER_VERTEX; i++) {
		boneTransform += bones[bonesID[i]] * bonesWeight[i];
	}
	if (!isAnimated)
		boneTransform = mat4(1.0);

	vec4 pos = boneTransform * vec4(aPos, 1.0);

	vec4 boneNormal = boneTransform * vec4(aNormal, 0);

	vs_out.TexCoords = aTexCoords;

	// calc TBN matrix to transforms vec from worldSpace to tangentSpace
	mat3 normalMatrix = transpose(inverse(mat3(model)));
	vec3 T = normalize(normalMatrix * aTangent);
	vec3 N = normalize(normalMatrix * aNormal);
	T = normalize(T - dot(T, N) * N);
	vec3 B = cross(N, T);

	mat3 TBN = transpose(mat3(T, B, N));

	vs_out.TangentLightDir = TBN * dirLight.direction;
	vs_out.TangentViewPos  = TBN * viewPos;
	vs_out.TangentFragPos  = TBN * vec3(modelScale * pos);

	vs_out.TangentNormal = TBN * mat3(transpose(inverse(modelScale))) * boneNormal.xyz;

	gl_Position = projection * view * model * modelScale * pos;
}
