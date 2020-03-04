#version 410 core

#define GAMMA 2.2

out vec4	fragColor;

in VS_OUT {
	vec2 TexCoords;
	vec3 TangentLightDir;
	vec3 TangentViewPos;
	vec3 TangentFragPos;
	vec3 TangentNormal;
} fs_in;

struct	ColorData {
	bool		isTexture;
	vec3		color;
	sampler2D	texture;
};

struct	Material {
	ColorData	diffuse;
	ColorData	specular;
	ColorData	normalMap;
	float		shininess;
};

struct DirLight {
	vec3		direction;

	vec3		ambient;
	vec3		diffuse;
	vec3		specular;
};

uniform Material	material;
uniform DirLight	dirLight;

vec3 calcDirLight(DirLight light, vec3 norm, vec3 viewDir) {
	vec3	lightDir = normalize(-fs_in.TangentLightDir);
	// diffuse
	float	diff = max(dot(lightDir, norm), 0.0);
	// specular
	vec3	halfwayDir = normalize(lightDir + viewDir);
	float	spec = pow(max(dot(norm, halfwayDir), 0.0), material.shininess * 4);

	// use texture or color for the diffuse
	vec3	ambient = light.ambient;
	vec3	diffuse = light.diffuse;
	if (material.diffuse.isTexture) {
		ambient *= vec3(texture(material.diffuse.texture, fs_in.TexCoords));
		diffuse *= diff * vec3(texture(material.diffuse.texture, fs_in.TexCoords));
	}
	else {
		ambient *= pow(material.diffuse.color, vec3(GAMMA));
		diffuse *= diff * pow(material.diffuse.color, vec3(GAMMA));
	}

	// use texture or color for the specular
	vec3 specular = light.specular;
	if (material.specular.isTexture)
		specular *= spec * vec3(texture(material.specular.texture, fs_in.TexCoords));
	else
		specular *= spec * pow(material.specular.color, vec3(GAMMA));

	return (ambient + diffuse + specular);
}

void main() {
	vec3	norm = normalize(fs_in.TangentNormal);
	if (material.normalMap.isTexture) {
		// obtain normal from normal map in range [0,1]
		norm  = texture(material.normalMap.texture, fs_in.TexCoords).rgb;
		// transform normal vector to range [-1,1]
		norm = normalize(norm * 2.0 - 1.0);  // this normal is in tangent space
	}

    vec3	viewDir = normalize(fs_in.TangentViewPos - fs_in.TangentFragPos);

	// Directional lighting
	vec3	result = calcDirLight(dirLight, norm, viewDir);

	fragColor = vec4(result, 1.0);
	// fragColor = vec4(0.2, 0.9, 0.2, 1.0);
	// if (material.normalMap.isTexture) {
	// 	fragColor = vec4(norm, 1.0);
	// }

	// apply gamma correction
    fragColor.rgb = pow(fragColor.rgb, vec3(1.0 / GAMMA));
}
