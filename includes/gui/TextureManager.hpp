#ifndef TEXTUREMANAGER_HPP_
#define TEXTUREMANAGER_HPP_

#include <array>

#include "utils/Shader.hpp"
#include "commonInclude.hpp"

#define NB_BLOCK_TYPES 5

namespace Block {
	enum Enum {
		B_FLOOR,
		SNAKE_BODY,
		SNAKE_HEAD,
		FOOD,
		WATER,
	};
}

class TextureManager {
	public:
		TextureManager();
		virtual ~TextureManager();
		TextureManager(TextureManager const &src);
		TextureManager &operator=(TextureManager const &rhs);

		void	setUniform(Shader &sh) const;
		void	activateTextures() const;
		void	disableTextures() const;

		// -- exceptions -------------------------------------------------------
		class TextureManagerException : public std::runtime_error {
			public:
				TextureManagerException();
				explicit TextureManagerException(const char* what_arg);
		};

	private:
		uint32_t	_textureAtlas;
		static std::array<std::array<int8_t, 6>, NB_BLOCK_TYPES> const	_blocks;
};

#endif  // TEXTUREMANAGER_HPP_
