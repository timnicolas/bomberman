#version 410 core

#define GAMMA 2.2
#define NIGHT mix(1.0, 0.05, nightProgress)
#define NIGHTFOG mix(1.0, 0.2, nightProgress)

out vec4	FragColor;

in VS_OUT {
	vec2 TexCoords;
	vec3 FragPos;
	vec3 Normal;
	flat int TextureId;
} fs_in;

struct	Material {
	vec3		specular;
	float		shininess;
};

struct DirLight {
	vec3		direction;

	vec3		ambient;
	vec3		diffuse;
	vec3		specular;
};

struct PointLight {
    bool		enabled;
    vec3		position;

    float		constant;
    float		linear;
    float		quadratic;

    vec3		ambient;
    vec3		diffuse;
    vec3		specular;
};

struct Fog {
	bool	enabled;
	int	maxDist;
	int	minDist;
	vec4	color;
};

uniform sampler2DArray	textureAtlas;

uniform bool		enableTransparency;
uniform vec3		viewPos;
uniform Material	material;
uniform DirLight	dirLight;
uniform PointLight	pointLight;
uniform Fog			fog = Fog(
	true,
	256,
	180,
	vec4(0.509, 0.8, 0.905, 1.0)
);
uniform float nightProgress = 0.0;

vec3 calcDirLight(DirLight light, vec3 norm, vec3 viewDir, vec3 color) {
	vec3	lightDir = normalize(-light.direction);
	// diffuse
	float	diff = max(dot(lightDir, norm), 0.0);  // result between 0 and 1
	// specular
	vec3 halfwayDir = normalize(lightDir + viewDir);
	float spec = pow(max(dot(norm, halfwayDir), 0.0), material.shininess * 4);

	// ambiant, diffuse
	vec3	ambient = light.ambient;
	vec3	diffuse = light.diffuse;

	ambient *= color;
	diffuse *= diff * color;

	// use texture or color for the specular
	vec3 specular = light.specular;
	specular *= spec * pow(material.specular, vec3(GAMMA));

	return ((ambient + diffuse + specular) * NIGHT);
}

vec3 calcPointLight(PointLight light, vec3 norm, vec3 fragPos, vec3 viewDir, vec3 color) {
	vec3    lightDir = normalize(light.position - fragPos);
	// diffuse
	float	diff = max(dot(norm, lightDir), 0.0);
	// specular
	vec3 halfwayDir = normalize(lightDir + viewDir);
	float spec = pow(max(dot(norm, halfwayDir), 0.0), material.shininess * 4);

	// ambiant, diffuse
	vec3	ambient = light.ambient;
	vec3	diffuse = light.diffuse;

	ambient *= color;
	diffuse *= diff * color;

	// specular
	vec3 specular = light.specular;
	specular *= spec * pow(material.specular, vec3(GAMMA));

	// attenuation
	float	distance = length(light.position - fragPos);
	float	attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

	ambient *= attenuation;
	diffuse *= attenuation;
	specular *= attenuation;
	return (ambient + diffuse + specular);
}

void main() {
	// discard if it's a transparent pixel
	if (enableTransparency && texture(textureAtlas, vec3(fs_in.TexCoords, fs_in.TextureId)).a < 0.4) {
		discard;
	}
	vec3 norm = normalize(fs_in.Normal);
	vec3 viewDir = normalize(viewPos - fs_in.FragPos);

	vec4 color = texture(textureAtlas, vec3(fs_in.TexCoords, fs_in.TextureId));

	// Directional lighting
	vec3 res = calcDirLight(dirLight, norm, viewDir, vec3(color));

	// PointLight
	if (pointLight.enabled) {
		res += calcPointLight(pointLight, norm, fs_in.FragPos, viewDir, vec3(color));
	}

	FragColor = vec4(res, color.a);

	// apply gamma correction
    FragColor.rgb = pow(FragColor.rgb, vec3(1.0 / GAMMA));

	// Calculate fog
	if (fog.enabled) {
		float dist = distance(viewPos, fs_in.FragPos);
		float fog_factor = (fog.maxDist - dist) / (fog.maxDist - fog.minDist);
		fog_factor = clamp(fog_factor, 0.0, 1.0);

		FragColor = mix(vec4(fog.color.rgb * NIGHTFOG, 1.0), FragColor, fog_factor);
	}

	// FragColor = vec4(0.95, 0.2, 0.3, 1.0);
}
