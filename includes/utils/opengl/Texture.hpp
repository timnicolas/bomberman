#ifndef TEXTURE_HPP_
#define TEXTURE_HPP_

#include <stdexcept>
#include "includesOpengl.hpp"

uint32_t	textureFromFile(const std::string path, bool inSpaceSRGB, bool pixelateOnZoom = true,
	int * width = nullptr, int * height = nullptr);
uint32_t	textureAtlasFromFile(const std::string path, bool inSpaceSRGB, \
	int tileSize, int layerCount);

class TextureFailToLoad : public std::exception {
	public:
		char const * what() const throw() {
			return "failed to load texture";
		}
};

#endif  // TEXTURE_HPP_
