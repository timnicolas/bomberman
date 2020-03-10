#ifndef MODEL_HPP_
#define MODEL_HPP_

#include "OpenGLModel.hpp"
#include "ETransform.hpp"

class Model {
	public:
		Model(OpenGLModel &openGLModel, float const &dtTime, ETransform transform = ETransform());
		virtual ~Model();
		Model(Model const &src);
		Model &operator=(Model const &rhs);

		void	draw();
		void	setAnimation(uint32_t id);
		void	setAnimation(std::string name);

		ETransform	transform;  // to move/scale/rotate the model
		bool		play;

	private:
		Model();  // private constructor, should not be called
		void	_updateAnimationTime();

		OpenGLModel &_openGLModel;
		float const	&_dtTime;
		uint32_t	_animationId;
		aiAnimation	*_curAnimation;
		float		_animationTime;
		float		_animationTimeTick;
		float		_animationSpeed;
};

#endif  // MODEL_HPP_
