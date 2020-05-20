#ifndef MODEL_HPP_
#define MODEL_HPP_

#include "OpenGLModel.hpp"
#include "ETransform.hpp"
#include "AEntity.hpp"

/**
 * @brief class to easy manage OpenGLModel,
 * pos/rot/scale transformations and animations settings
 *
 */
class Model {
	public:
		Model(OpenGLModel &openGLModel, float const &dtTime, ETransform transform = ETransform());
		virtual ~Model();
		Model(Model const &src);
		Model &operator=(Model const &rhs);

		void	draw();

		// anim end callback
		typedef void (AEntity::*AnimEndCb)(std::string animName);  /**< animation name */

		void	setAnimation(uint32_t id, AnimEndCb animEndCbFunc = nullptr,
			AEntity *animEndCbClass = nullptr);
		void	setAnimation(std::string name, AnimEndCb animEndCbFunc = nullptr,
			AEntity *animEndCbClass = nullptr);
		void	setNextAnimation(AnimEndCb animEndCbFunc = nullptr,
			AEntity *animEndCbClass = nullptr);
		void	setAnimProgress(float progress);
		void	setAnimCurrentTime(float animTime);
		void	setMeshTexture(TextureType::Enum type, std::string const meshName,
			std::string const path, bool inSpaceSRGB = true);

		float	getAnimCurrentTime() const;
		float	getAnimProgress() const;
		float	getAnimDuration() const;
		void	printAnimsNames() const;
		void	printMeshesNames() const;
		std::string getCurrentAnimationName() const;

		ETransform	transform;  /**< To move/scale/rotate the model */
		bool		play;  /**< Play animation */
		bool		loopAnimation;  /**< If animation repeat */
		float		animationSpeed;  /**< Animation speed */

	private:
		Model();  // private constructor, should not be called
		void	_updateAnimationTime();
		void	_updateTicksPerSecond();

		/**
		 * @brief Mesh texture
		 */
		struct	MeshTexture {
			TextureType::Enum	type;  /**< Texture type */
			std::string			meshName;  /**< Mesh name */
			std::string			path;  /**< Texture path */
			bool				inSpaceSRGB;  /**< If is in space SRGB */
		};

		OpenGLModel &_openGLModel;  /**< A ref to openGLModel object */
		float const	&_dtTime;  /**< A ref to the delta time (ms) */
		uint32_t	_animationId;  /**< Current animation ID */
		aiAnimation	*_curAnimation;  /**< Current animation */
		float		_ticksPerSecond;  /**< Tick per second */
		float		_animationTime;  /**< Animation time in ms */
		float		_animationTimeTick;  /**< Cur animation time in tick */
		AnimEndCb	_animEndCbFunc;  /**< cb function called on animation end */
		AEntity		*_animEndCbClass;  /**< cb class called on animation end */
		std::vector<Model::MeshTexture>	_meshTextures;  /**< store manually modified mesh textures */
};

#endif  // MODEL_HPP_
