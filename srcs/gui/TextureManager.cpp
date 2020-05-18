#include "TextureManager.hpp"

#include <fstream>

#include "utils/opengl/Texture.hpp"
#include "Logging.hpp"

/**
 * @brief Construct a new Texture Manager:: Texture Manager object
 */
TextureManager::TextureManager()
: _textureAtlas(0) {
	// load texture atlas
	std::string path = "./bomberman-assets/textures/textures.png";
	bool inSpaceSRGB = true;
	try {
		_textureAtlas = textureAtlasFromFile(path, inSpaceSRGB, 32, 256);
	}
	catch(TextureException const & e) {
		throw TextureManagerException("failed to load texture atlas");
	}
}

/**
 * @brief Destroy the Texture Manager:: Texture Manager object
 */
TextureManager::~TextureManager() {
	// release openGl texture
	if (_textureAtlas != 0) {
		glDeleteTextures(1, &_textureAtlas);
	}
}

/**
 * @brief Construct a new Texture Manager:: Texture Manager object
 *
 * @param src The object to do the copy
 */
TextureManager::TextureManager(TextureManager const &src)
: _textureAtlas(0) {
	*this = src;
}

/**
 * @brief Copy this object
 *
 * @param rhs The object to copy
 * @return TextureManager& A reference to the copied object
 */
TextureManager &TextureManager::operator=(TextureManager const &rhs) {
	(void)rhs;
	return *this;
}

/**
 * @brief set the uniform for the textureAtlas in the shader
 *
 * @param sh the shader
 */
void	TextureManager::setUniform(Shader &sh) const {
	// activate textures
	sh.setInt("textureAtlas", 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D_ARRAY, _textureAtlas);

	// set textures uniform
	uint8_t	i = 0;
	for (std::array<int8_t, 6> const &block : _blocks) {
		for (uint8_t j = 0; j < 6; ++j) {
			sh.setInt("blockTextures[" + std::to_string(i * 6 + j) + "]", block[j]);
		}
		++i;
	}

	glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
}

/**
 * @brief bind the texture atlas
 */
void	TextureManager::activateTextures() const {
	// activate textures
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D_ARRAY, _textureAtlas);
}

/**
 * @brief unbind the texture atlas
 */
void	TextureManager::disableTextures() const {
	// disable textures
	glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
}

// -- exceptions ---------------------------------------------------------------
TextureManager::TextureManagerException::TextureManagerException()
: std::runtime_error("[TextureManagerException]") {}

TextureManager::TextureManagerException::TextureManagerException(const char* what_arg)
: std::runtime_error(std::string(std::string("[TextureManagerException] ") + what_arg).c_str()) {}

// -- statics const ------------------------------------------------------------
std::array<std::array<int8_t, 6>, NB_BLOCK_TYPES> const	TextureManager::_blocks = {{
	{{0, 0, 0, 0, 0, 0}},  // floor
	{{9, 10, 10, 10, 11, 8}},  // player
	{{10, 10, 10, 10, 10, 10}},  // ia
	{{5, 6, 3, 6, 7, 4}},  // bomb
	{{0, 0, 0, 0, 1, 0}},  // durable_wall
	{{2, 2, 2, 2, 2, 2}},  // destructible_wall
	{{3, 3, 3, 3, 3, 3}},  // fire
	{{4, 4, 4, 4, 4, 4}},  // end
	{{8, 8, 8, 8, 8, 8}},  // end_open
	{{5, 5, 5, 5, 5, 5}},  // flag
	{{1, 1, 1, 1, 1, 1}},  // block
	{{12, 12, 12, 12, 12, 12}},  // bonus life
	{{13, 13, 13, 13, 13, 13}},  // bonus bombs
	{{14, 14, 14, 14, 14, 14}},  // bonus flame
	{{15, 15, 15, 15, 15, 15}},  // bonus speed
	{{16, 16, 16, 16, 16, 16}},  // bonus wallpass
	{{17, 17, 17, 17, 17, 17}},  // bonus detonator
	{{18, 18, 18, 18, 18, 18}},  // bonus bombpass
	{{19, 19, 19, 19, 19, 19}},  // bonus flampass
	{{20, 20, 20, 20, 20, 20}},  // bonus shield
	{{21, 21, 21, 21, 21, 21}},  // bonus time
	{{22, 22, 22, 22, 22, 22}},  // bonus points
}};
