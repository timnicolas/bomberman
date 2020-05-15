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
		typedef void (AEntity::*AnimEndCb)(std::string animName);

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
		std::string getCurrentAnimationName() const;

		ETransform	transform;  // to move/scale/rotate the model
		bool		play;
		bool		loopAnimation;
		float		animationSpeed;

	private:
		Model();  // private constructor, should not be called
		void	_updateAnimationTime();
		void	_updateTicksPerSecond();

		struct	MeshTexture {
			TextureType::Enum	type;
			std::string			meshName;
			std::string			path;
			bool				inSpaceSRGB;
		};

		OpenGLModel &_openGLModel;
		float const	&_dtTime;
		uint32_t	_animationId;
		aiAnimation	*_curAnimation;
		float		_ticksPerSecond;
		float		_animationTime;  // in ms
		float		_animationTimeTick;
		AnimEndCb	_animEndCbFunc;  // cb function called on animation end
		AEntity		*_animEndCbClass;  // cb class called on animation end
		std::vector<Model::MeshTexture>	_meshTextures;  // store manually modified mesh textures
};

#endif  // MODEL_HPP_
