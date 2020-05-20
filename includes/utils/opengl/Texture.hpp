#ifndef TEXTURE_HPP_
#define TEXTURE_HPP_

#include <assimp/scene.h>
#include <stdexcept>
#include "includesOpengl.hpp"

uint32_t	_createTexture(uint8_t *data, std::string const name, bool inSpaceSRGB,
	int formatId, glm::ivec2 size, GLint glTexMinFilter = GL_NEAREST_MIPMAP_LINEAR,
	GLint glTexMagFilter = GL_NEAREST, bool isAtlas = false, int tileSize = 0,
	int layerCount = 0);

uint32_t	textureFromFile(std::string const path, bool inSpaceSRGB,
	glm::ivec2 *sizeOut = nullptr, GLint glTexMinFilter = GL_NEAREST_MIPMAP_LINEAR,
	GLint glTexMagFilter = GL_NEAREST);

uint32_t	textureAtlasFromFile(std::string const path, bool inSpaceSRGB, \
	int tileSize, int layerCount, GLint glTexMinFilter = GL_NEAREST_MIPMAP_LINEAR,
	GLint glTexMagFilter = GL_NEAREST);

uint32_t	textureFromFbx(aiScene const *scene, int locationId, bool inSpaceSRGB,
	GLint glTexMinFilter = GL_NEAREST_MIPMAP_LINEAR, GLint glTexMagFilter = GL_NEAREST);

// Exceptions
/**
 * @brief Texture exception
 */
class TextureException : public std::runtime_error {
	public:
		TextureException();
		explicit TextureException(const char* what_arg);
};

#endif  // TEXTURE_HPP_
