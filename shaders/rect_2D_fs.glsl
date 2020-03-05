#version 410 core

out vec4		FragColor;

uniform vec4	masterColor;
uniform vec4	secondColor;
uniform float	colorFactor;

void main()
{
	FragColor = mix(secondColor, masterColor, colorFactor);
	if (FragColor.a < 0.2)
		discard;
}