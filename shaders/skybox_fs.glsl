#version 410 core

#define NIGHTFOG mix(1.0, 0.2, nightProgress)

out vec4 fragColor;

in vec3 texCoords;

uniform samplerCube skybox;
uniform float nightProgress = 0.0;

void main() {
    fragColor = vec4(texture(skybox, texCoords).rgb * NIGHTFOG, 1.0);
}