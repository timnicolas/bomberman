#version 410 core

layout (location = 0) in vec2	aPos;
layout (location = 1) in vec2	aTexCoord;
layout (location = 2) in float	aTexID;

out vec2	TexCoords;
out float	TexID;

uniform mat4 projection;

void main()
{
    gl_Position = projection * vec4(aPos, 0.0, 1.0);
    TexCoords = aTexCoord;
	TexID = aTexID;
}