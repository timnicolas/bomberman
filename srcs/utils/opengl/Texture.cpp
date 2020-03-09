#define STB_IMAGE_IMPLEMENTATION
#include "libs/stb_image.h"
#include "Texture.hpp"
#include "Logging.hpp"

// -- textureFromFile ----------------------------------------------------------
/**
 * @brief load image from file and convert it to an OpenGl texture
 *
 * @param path image file location
 * @param inSpaceSRGB define if image are in sRGB space (Gamma correction)
 * @param sizeOut specify a size pointer to retrieve it later
 * @param glTexMinFilter OpenGl zoom out behaviour flag
 * @param glTexMagFilter OpenGl zoom in behaviour flag
 * @return uint32_t OpenGl texture id
 */
uint32_t	textureFromFile(std::string const path, bool inSpaceSRGB, glm::ivec2 *sizeOut,
	GLint glTexMinFilter, GLint glTexMagFilter)
{
	int			formatId;
	glm::ivec2	size;
	u_char		*data;

	// if the caller whant to retrieve the real image size
	if (sizeOut != nullptr) {
		// load texture data
		data = stbi_load(path.c_str(), &(sizeOut->x), &(sizeOut->y), &formatId, 0);
		return _createTexture(data, path, inSpaceSRGB, formatId, *sizeOut,
			glTexMinFilter, glTexMagFilter);
	}

	// load texture data
	data = stbi_load(path.c_str(), &(size.x), &(size.y), &formatId, 0);
	return _createTexture(data, path, inSpaceSRGB, formatId, *sizeOut,
		glTexMinFilter, glTexMagFilter);
}

// -- textureAtlasFromFile -----------------------------------------------------
/**
 * @brief load texture atlas from file and convert it to an OpenGl texture array
 *
 * @param path image file location
 * @param inSpaceSRGB define if image are in sRGB space (Gamma correction)
 * @param tileSize size of one tile
 * @param layerCount number of tiles
 * @param glTexMinFilter OpenGl zoom out behaviour flag
 * @param glTexMagFilter OpenGl zoom in behaviour flag
 * @return uint32_t OpenGl texture id
 */
uint32_t	textureAtlasFromFile(std::string const path, bool inSpaceSRGB,
	int tileSize, int layerCount, GLint glTexMinFilter, GLint glTexMagFilter)
{
	int			formatId;
	glm::ivec2	size;
	uint8_t		*data;

	// load texture data
	data = stbi_load(path.c_str(), &(size.x), &(size.y), &formatId, 0);

	return _createTexture(data, path, inSpaceSRGB, formatId, size, glTexMinFilter,
		glTexMagFilter, true, tileSize, layerCount);
}

// -- textureFromFbx -----------------------------------------------------------
/**
 * @brief load texture from fbx model and convert it to an OpenGl texture
 *
 * @param scene Assimp scene to load the texture from
 * @param locationId Assimp texture location id
 * @param inSpaceSRGB define if image are in sRGB space (Gamma correction)
 * @param glTexMinFilter OpenGl zoom out behaviour flag
 * @param glTexMagFilter OpenGl zoom in behaviour flag
 * @return uint32_t OpenGl texture id
 */
uint32_t	textureFromFbx(aiScene const *scene, int locationId, bool inSpaceSRGB,
	GLint glTexMinFilter, GLint glTexMagFilter)
{
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

	return _createTexture(data, "from fbx", inSpaceSRGB, formatId, size,
		glTexMinFilter, glTexMagFilter);
}

// -- _createTexture -----------------------------------------------------------
uint32_t	_createTexture(uint8_t *data, std::string const name, bool inSpaceSRGB,
	int formatId, glm::ivec2 size, GLint glTexMinFilter, GLint glTexMagFilter,
	bool isAtlas, int tileSize, int layerCount)
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
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, glTexMinFilter);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, glTexMagFilter);

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
