#include "Texture.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include "libs/stb_image.h"
#include "Logging.hpp"

/**
 * @brief Load a texture from a file
 *
 * @param path The texture path
 * @param inSpaceSRGB inSpaceSRGB option
 * @param pixelateOnZoom Option to pixelate or show a smooth transition when zoom on an image
 * @param width Pointer on int. If not nullptr, setted to the base width of the loaded texture
 * @param height Pointer on int. If not nullptr, setted to the base height of the loaded texture
 * @return uint32_t The texture ID
 */
uint32_t	textureFromFile(const std::string path, bool inSpaceSRGB, bool pixelateOnZoom, int * width, int * height) {
	uint32_t	textureID;
    int			nrComponents;
    int			width_;
    int			height_;
	u_char		*data;
	GLint		intFormat;
	GLenum		format;


	data = stbi_load(path.c_str(), &width_, &height_, &nrComponents, 0);

	if (data) {
		glGenTextures(1, &textureID);
		if (nrComponents == 1) {
			intFormat = GL_RED;
			format = GL_RED;
		}
		else if (nrComponents == 3) {
			intFormat = inSpaceSRGB ? GL_SRGB : GL_RGB;
			format = GL_RGB;
		}
		else if (nrComponents == 4) {
			intFormat = inSpaceSRGB ? GL_SRGB_ALPHA : GL_RGBA;
			format = GL_RGBA;
		}
		else {
			logErr("failed to load texture " << path);
			throw TextureFailToLoad();
		}

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, intFormat, width_, height_, 0, format, \
		GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_BASE_LEVEL, 0);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAX_LEVEL, 5);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		if (pixelateOnZoom) {
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		}
		else {
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		}

		stbi_image_free(data);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	else {
		logErr("Texture failed to load at path: " << path);
		stbi_image_free(data);
		throw TextureFailToLoad();
	}

	if (width) *width = width_;
	if (height) *height = height_;

    return textureID;
}

/**
 * @brief Load a texture atlas from a file
 *
 * @param path The texture path
 * @param inSpaceSRGB inSpaceSRGB option
 * @param tileSize The size of an element
 * @param layerCount The total number of elements
 * @return uint32_t The texture ID
 */
uint32_t	textureAtlasFromFile(const std::string path, bool inSpaceSRGB, int tileSize, int layerCount) {
	uint32_t	textureID;
    int			nrComponents;
    int			width;
    int			height;
	u_char		*data;
	GLint		intFormat;
	GLenum		format;

	data = stbi_load(path.c_str(), &width, &height, &nrComponents, 0);

	if (data) {
		glGenTextures(1, &textureID);
		if (nrComponents == 1) {
			intFormat = GL_RED;
			format = GL_RED;
		}
		else if (nrComponents == 3) {
			intFormat = inSpaceSRGB ? GL_SRGB : GL_RGB;
			format = GL_RGB;
		}
		else if (nrComponents == 4) {
			intFormat = inSpaceSRGB ? GL_SRGB_ALPHA : GL_RGBA;
			format = GL_RGBA;
		}
		else {
			logErr("failed to load texture " << path);
			throw TextureFailToLoad();
		}

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D_ARRAY, textureID);
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

		glGenerateMipmap(GL_TEXTURE_2D_ARRAY);

		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAX_LEVEL, 5);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		stbi_image_free(data);
		glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
	}
	else {
		logErr("Texture failed to load at path: " << path);
		stbi_image_free(data);
		throw TextureFailToLoad();
	}

    return textureID;
}
