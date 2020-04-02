#version 410 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTextCoord;

out vec2 TexCoords;

uniform mat4 projection;

void main()
{
    gl_Position = projection * vec4(aPos, 1.0);
    TexCoords = aTextCoord;
}