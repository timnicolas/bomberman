#ifndef TEXTURE_HPP_
#define TEXTURE_HPP_

#include <assimp/scene.h>
#include <stdexcept>
#include "includesOpengl.hpp"

uint32_t	createTexture(uint8_t *data, std::string const name, bool inSpaceSRGB,
	int formatId, glm::ivec2 size, bool isAtlas = false, int tileSize = 0,
	int layerCount = 0);
uint32_t	textureFromFile(std::string const path, bool inSpaceSRGB);
uint32_t	textureAtlasFromFile(std::string const path, bool inSpaceSRGB, \
	int tileSize, int layerCount);
uint32_t	textureFromFbx(aiScene const *scene, int locationId, bool inSpaceSRGB);

// Exceptions
class TextureException : public std::runtime_error {
	public:
		TextureException();
		explicit TextureException(const char* what_arg);
};

#endif  // TEXTURE_HPP_
