#ifndef MODEL_HPP_
#define MODEL_HPP_

#include "OpenGLModel.hpp"
#include "ETransform.hpp"

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
		void	setAnimation(uint32_t id);
		void	setAnimation(std::string name);
		void	setNextAnimation();
		void	setAnimProgress(float progress);
		void	setAnimCurrentTime(float animTime);

		float	getAnimCurrentTime() const;
		float	getAnimProgress() const;
		float	getAnimDuration() const;

		ETransform	transform;  // to move/scale/rotate the model
		bool		play;
		bool		loopAnimation;
		float		animationSpeed;

	private:
		Model();  // private constructor, should not be called
		void	_updateAnimationTime();
		void	_updateTicksPerSecond();

		OpenGLModel &_openGLModel;
		float const	&_dtTime;
		uint32_t	_animationId;
		aiAnimation	*_curAnimation;
		float		_ticksPerSecond;
		float		_animationTime;  // in ms
		float		_animationTimeTick;
};

#endif  // MODEL_HPP_
