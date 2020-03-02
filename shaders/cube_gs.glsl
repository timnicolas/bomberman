#version 410 core

layout (points) in;
layout (triangle_strip, max_vertices = 4) out;

in GS_IN {
	flat int FaceId;
	vec3 Normal;
	flat int TextureId;
} gs_in[];

out VS_OUT {
	vec2 TexCoords;
	vec3 FragPos;
	vec3 Normal;
	flat int TextureId;
} gs_out;

uniform vec3 blockSize = vec3(1.0, 1.0, 1.0);

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void buildFace(vec4 bottomLeft, vec4 tl, vec4 br, vec2 fSize) {
	// topLeft
	gl_Position = projection * view * model * (bottomLeft + tl);
	gs_out.TexCoords = vec2(0, 0);
	gs_out.FragPos = vec3(model * (bottomLeft + tl));
	gs_out.Normal = gs_in[0].Normal;
	gs_out.TextureId = gs_in[0].TextureId;
	EmitVertex();
	// bottomLeft
	gl_Position = projection * view * model * bottomLeft;
	gs_out.TexCoords = vec2(0, fSize.y);
	gs_out.FragPos = vec3(model * bottomLeft);
	gs_out.Normal = gs_in[0].Normal;
	gs_out.TextureId = gs_in[0].TextureId;
	EmitVertex();
	// topRight
	gl_Position = projection * view * model * (bottomLeft + tl + br);
	gs_out.TexCoords = vec2(fSize.x, 0);
	gs_out.FragPos = vec3(model * (bottomLeft + tl + br));
	gs_out.Normal = gs_in[0].Normal;
	gs_out.TextureId = gs_in[0].TextureId;
	EmitVertex();
	// bottomRight
	gl_Position = projection * view * model * (bottomLeft + br);
	gs_out.TexCoords = vec2(fSize.x, fSize.y);
	gs_out.FragPos = vec3(model * (bottomLeft + br));
	gs_out.Normal = gs_in[0].Normal;
	gs_out.TextureId = gs_in[0].TextureId;
	EmitVertex();
	EndPrimitive();
}

void main() {
	vec2 fSize;

	// retrieve face size
	if (gs_in[0].FaceId == 0 || gs_in[0].FaceId == 2) {
		fSize = vec2(blockSize.x, blockSize.y);
	}
	else if (gs_in[0].FaceId == 1 || gs_in[0].FaceId == 3) {
		fSize = vec2(blockSize.z, blockSize.y);
	}
	else if (gs_in[0].FaceId == 4 || gs_in[0].FaceId == 5) {
		fSize = vec2(blockSize.x, blockSize.z);
	}

	vec3 tl[6] = vec3[6](
		vec3(0, fSize.y, 0),
		vec3(0, fSize.y, 0),
		vec3(0, fSize.y, 0),
		vec3(0, fSize.y, 0),
		vec3(0, 0, -fSize.y),
		vec3(0, 0, fSize.y)
	);

	vec3 br[6] = vec3[6](
		vec3(fSize.x, 0, 0),
		vec3(0, 0, -fSize.x),
		vec3(-fSize.x, 0, 0),
		vec3(0, 0, fSize.x),
		vec3(fSize.x, 0, 0),
		vec3(fSize.x, 0, 0)
	);

	buildFace(gl_in[0].gl_Position, vec4(tl[gs_in[0].FaceId], 0),
		vec4(br[gs_in[0].FaceId], 0), fSize);
}
