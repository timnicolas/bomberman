#version 410 core

in vec2 	TexCoords;

out vec4	FragColor;

uniform sampler2D	text;
uniform vec4		color;

void main()
{
	FragColor = mix(texture(text, TexCoords), vec4(color.rgb, 1), color.a);
	if (FragColor.a < 0.2)
		discard;
}