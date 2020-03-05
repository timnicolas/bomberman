#version 410 core

in vec2 	TexCoords;
in float	TexID;

out vec4	FragColor;

uniform sampler2DArray	textureAtlas;
uniform vec4			color;

void main()
{
	FragColor = mix(vec4(color.rgb, 1), texture(textureAtlas, vec3(TexCoords, TexID)), color.a);
}