#include "Model.hpp"

// -- Constructors -------------------------------------------------------------
Model::Model(OpenGLModel &openGLModel, float const &dtTime, ETransform transform)
: transform(transform),
  _openGLModel(openGLModel),
  _dtTime(dtTime)
{
	play = false;
	_animationId = 0;
	_animationTime = 0;
	_animationSpeed = 1.0;

	_curAnimation = nullptr;
	if (_openGLModel.isAnimated()) {
		_curAnimation = _openGLModel.getAiAnimation(_animationId);
	}
}

Model::~Model() {
}

Model::Model(Model const &src)
: transform(src.transform),
  _openGLModel(src._openGLModel),
  _dtTime(src._dtTime) {
	*this = src;
}

Model &Model::operator=(Model const &rhs) {
	if (this != &rhs) {
		play = rhs.play;
		_animationId = rhs._animationId;
		_curAnimation = rhs._curAnimation;
		_animationTime = rhs._animationTime;
		_animationTimeTick = rhs._animationTimeTick;
		_animationSpeed = rhs._animationSpeed;
	}
	return *this;
}

// -- draw ---------------------------------------------------------------------
void	Model::draw() {
	// the model is animated
	if (_openGLModel.isAnimated() && _curAnimation != nullptr) {
		// change openGLModel current animation
		_openGLModel.setAnimation(_animationId);

		logDebug("_animationId: " << _animationId);

		if (play) {
			_updateAnimationTime();
		}
	}

	// update openGLModel model matrix
	_openGLModel.setModel(transform.getModel());

	logDebug("Model::draw()");

	// render the model
	_openGLModel.draw(_animationTimeTick);
}

// -- _updateAnimationTime -----------------------------------------------------
void	Model::_updateAnimationTime() {
	// update _animationTime
	_animationTime += 1000 * _dtTime * _animationSpeed;
	// retrieve the animation ticks/second
	float ticksPerSecond = (_curAnimation->mTicksPerSecond != 0)
		? _curAnimation->mTicksPerSecond : 25.0f;
	// convert _animationTime to tick time
	float timeInTicks = (_animationTime / 1000.0) * ticksPerSecond;
	// loops the animation
	_animationTimeTick = fmod(timeInTicks, _curAnimation->mDuration);
}

// -- setAnimation -------------------------------------------------------------
void	Model::setAnimation(uint32_t id) {
	if (_openGLModel.setAnimation(id)) {
		_animationId = id;
		_curAnimation = _openGLModel.getAiAnimation(_animationId);
		_animationTime = 0;  // reset animation time
	}
}

// -- setAnimation -------------------------------------------------------------
void	Model::setAnimation(std::string name) {
	_openGLModel.getAnimationId(name, _animationId);
	_curAnimation = _openGLModel.getAiAnimation(_animationId);
	_animationTime = 0;  // reset animation time
}
