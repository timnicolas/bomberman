#ifndef TEXTUREMANAGER_HPP_
#define TEXTUREMANAGER_HPP_

#include <array>

#include "Shader.hpp"
#include "includesOpengl.hpp"

#define NB_BLOCK_TYPES 9

namespace Block {
	enum Enum {
		FLOOR,
		PLAYER,
		IA,
		BOMB,
		DURABLE_WALL,
		DESTRUCTIBLE_WALL,
		FIRE,
		END,
		FLAG,
	};
}  // namespace Block

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
