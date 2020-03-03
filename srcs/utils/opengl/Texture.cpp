#define STB_IMAGE_IMPLEMENTATION
#include "libs/stb_image.h"
#include "Texture.hpp"
#include "Logging.hpp"

// -- textureFromFile ----------------------------------------------------------
uint32_t	textureFromFile(std::string const path, bool inSpaceSRGB) {
	int			formatId;
	glm::ivec2	size;
	u_char		*data;

	// load texture data
	data = stbi_load(path.c_str(), &(size.x), &(size.y), &formatId, 0);

	return createTexture(data, path, inSpaceSRGB, formatId, size);
}

// -- textureAtlasFromFile -----------------------------------------------------
uint32_t	textureAtlasFromFile(std::string const path, bool inSpaceSRGB,
	int tileSize, int layerCount)
{
	int			formatId;
	glm::ivec2	size;
	uint8_t		*data;

	// load texture data
	data = stbi_load(path.c_str(), &(size.x), &(size.y), &formatId, 0);

	return createTexture(data, path, inSpaceSRGB, formatId, size, true, tileSize, layerCount);
}

// -- textureFromFbx -----------------------------------------------------------
uint32_t	textureFromFbx(aiScene const *scene, int locationId, bool inSpaceSRGB) {
	int			formatId;
	glm::ivec2	size;
	aiTexture	*texture = nullptr;
	u_char		*data;

	// retrieve texture
	texture = scene->mTextures[locationId];

	// load compressed texture data
	if (texture->mHeight == 0) {
		data = stbi_load_from_memory(reinterpret_cast<unsigned char*>(texture->pcData), \
			texture->mWidth, &(size.x), &(size.y), &formatId, 0);
	}
	// load uncompressed texture data
	else {
		data = stbi_load_from_memory(reinterpret_cast<unsigned char*>(texture->pcData), \
			texture->mWidth * texture->mHeight, &(size.x), &(size.y), &formatId, 0);
	}

	return createTexture(data, "from fbx", inSpaceSRGB, formatId, size);
}

// -- createTexture ------------------------------------------------------------
uint32_t	createTexture(uint8_t *data, std::string const name, bool inSpaceSRGB,
	int formatId, glm::ivec2 size, bool isAtlas, int tileSize, int layerCount)
{
	uint32_t	textureID;
	GLint		intFormat;
	GLenum		format;

	if (data) {
		glGenTextures(1, &textureID);
		if (formatId == 1) {
			intFormat = GL_RED;
			format = GL_RED;
		}
		else if (formatId == 3) {
			intFormat = inSpaceSRGB ? GL_SRGB : GL_RGB;
			format = GL_RGB;
		}
		else if (formatId == 4) {
			intFormat = inSpaceSRGB ? GL_SRGB_ALPHA : GL_RGBA;
			format = GL_RGBA;
		}
		else {
			throw TextureException(std::string("invalid texture format: " + name).c_str());
		}

		glActiveTexture(GL_TEXTURE0);
		// normal texture
		if (!isAtlas) {
			glBindTexture(GL_TEXTURE_2D, textureID);
			// create texture
			glTexImage2D(GL_TEXTURE_2D, 0, intFormat, size.x, size.y, 0, format, \
			GL_UNSIGNED_BYTE, data);

			// generate mipmap
			glGenerateMipmap(GL_TEXTURE_2D);
			glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_BASE_LEVEL, 0);
			glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAX_LEVEL, 5);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

			glBindTexture(GL_TEXTURE_2D, 0);
		}
		// texture atlas
		else {
			glBindTexture(GL_TEXTURE_2D_ARRAY, textureID);
			// create texture
			glTexImage3D(GL_TEXTURE_2D_ARRAY,
				0,						// mipmap level
				intFormat,				// gpu texel format
				tileSize,				// width
				tileSize,				// height
				layerCount,				// depth
				0,						// border
				format,					// cpu pixel format
				GL_UNSIGNED_BYTE,		// cpu pixel coord type
				data);					// pixel data

			// generate mipmap
			glGenerateMipmap(GL_TEXTURE_2D_ARRAY);
			glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_BASE_LEVEL, 0);
			glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAX_LEVEL, 5);
			glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

			glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
		}
		stbi_image_free(data);
	}
	else {
		stbi_image_free(data);
		throw TextureException(std::string("failed to load texture: " + name).c_str());
	}

	return textureID;
}

// -- Exceptions errors --------------------------------------------------------

TextureException::TextureException()
: std::runtime_error("TextureException") {}

TextureException::TextureException(const char* what_arg)
: std::runtime_error(std::string(std::string("TextureException: ") + what_arg).c_str()) {}
