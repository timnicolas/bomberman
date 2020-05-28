#version 410 core

out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D sceneTex;
uniform bool horizontal = true;
// gausian blur weight
uniform float weight[5] = float[] (0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);

void	main() {
	if (gl_FragCoord.y > 400) {
		FragColor = texture(sceneTex, TexCoords);
	}
	else {
		// gets size of single texel
		vec2 texOffset = 1.0 / textureSize(sceneTex, 0);
		// current fragment's contribution
		vec3 result = texture(sceneTex, TexCoords).rgb * weight[0];

		if (horizontal) {
			for (int i = 1; i < 5; ++i) {
				result += texture(sceneTex, TexCoords + vec2(texOffset.x * i, 0.0)).rgb * weight[i];
				result += texture(sceneTex, TexCoords - vec2(texOffset.x * i, 0.0)).rgb * weight[i];
			}
		}
		else {
			for (int i = 1; i < 5; ++i) {
				result += texture(sceneTex, TexCoords + vec2(0.0, texOffset.y * i)).rgb * weight[i];
				result += texture(sceneTex, TexCoords - vec2(0.0, texOffset.y * i)).rgb * weight[i];
			}
		}

		FragColor = vec4(result, 1.0);
	}
}
