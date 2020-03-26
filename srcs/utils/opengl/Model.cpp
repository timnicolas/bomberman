#include "Model.hpp"

// -- Constructors -------------------------------------------------------------
Model::Model(OpenGLModel &openGLModel, float const &dtTime, ETransform transform)
: transform(transform),
  _openGLModel(openGLModel),
  _dtTime(dtTime)
{
	play = false;
	loopAnimation = false;
	_animationId = 0;
	_animationTime = 0;
	animationSpeed = 1.0;

	_curAnimation = nullptr;
	_ticksPerSecond = 0.0;
	if (_openGLModel.isAnimated()) {
		_curAnimation = _openGLModel.getAiAnimation(_animationId);
		_updateTicksPerSecond();
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
		loopAnimation = rhs.loopAnimation;
		_animationId = rhs._animationId;
		_curAnimation = rhs._curAnimation;
		_ticksPerSecond = rhs._ticksPerSecond;
		_animationTime = rhs._animationTime;
		_animationTimeTick = rhs._animationTimeTick;
		animationSpeed = rhs.animationSpeed;
	}
	return *this;
}

// -- draw ---------------------------------------------------------------------
/**
 * @brief draw a 3d model animated or not
 *
 */
void	Model::draw() {
	// the model is animated
	if (_openGLModel.isAnimated() && _curAnimation != nullptr) {
		// change openGLModel current animation
		_openGLModel.setAnimation(_animationId);

		if (play) {
			_updateAnimationTime();
		}
	}

	// update openGLModel model matrix
	_openGLModel.setModel(transform.getModel());

	// render the model
	_openGLModel.draw(_animationTimeTick);
}

// -- setAnimation -------------------------------------------------------------
/**
 * @brief change the animation by id
 *
 * @param id the animation id
 */
void	Model::setAnimation(uint32_t id) {
	if (_openGLModel.setAnimation(id)) {
		_animationId = id;
		_curAnimation = _openGLModel.getAiAnimation(_animationId);
		_updateTicksPerSecond();
		_animationTime = 0;  // reset animation time
	}
}

// -- setAnimation -------------------------------------------------------------
/**
 * @brief change the animation by name
 *
 * @param name the animation name
 */
void	Model::setAnimation(std::string name) {
	_openGLModel.getAnimationId(name, _animationId);
	_curAnimation = _openGLModel.getAiAnimation(_animationId);
	_updateTicksPerSecond();
	_animationTime = 0;  // reset animation time
}

// -- setNextAnimation ---------------------------------------------------------
/**
 * @brief change the animation to the next one
 *
 */
void	Model::setNextAnimation() {
	uint32_t nextId = _animationId + 1;

	if (nextId >= _openGLModel.getNbAnimations()) {
		nextId = 0;
	}

	setAnimation(nextId);
}

// -- setAnimProgress ----------------------------------------------------------
/**
 * @brief set the animation current time by percent
 *
 * @param progress animation progress percent, between 0 and 1
 */
void	Model::setAnimProgress(float progress) {
	if (_openGLModel.isAnimated()) {
		if (loopAnimation) {
			progress = fmod(progress, 1.0f);
		}
		else {
			progress = std::clamp(progress, 0.0f, 1.0f);
		}

		_animationTime = (_curAnimation->mDuration / _ticksPerSecond) * 1000 * progress;
	}
	else {
		logWarn("the model have no animation");
	}
}
// -- setAnimCurrentTime -------------------------------------------------------
/**
 * @brief set the animation current time by ms
 *
 * @param animTime animation current time in ms
 */
void	Model::setAnimCurrentTime(float animTime) {
	if (_openGLModel.isAnimated()) {
		if (loopAnimation) {
			_animationTime = fmod(animTime, getAnimDuration());
		}
		else {
			_animationTime = std::clamp(animTime, 0.0f, getAnimDuration());
		}
	}
	else {
		logWarn("the model have no animation");
	}
}

// -- getters ------------------------------------------------------------------
/**
 * @brief get the curent animation time in ms
 *
 * @return float, time in ms between 0 and animationDuration
 */
float	Model::getAnimCurrentTime() const {
	if (_openGLModel.isAnimated()) {
		return _animationTime;
	}

	logWarn("the model have no animation");
	return 0;
}

/**
 * @brief get the curent animation progress in percentage
 *
 * @return float, a value between 0 and 1
 */
float	Model::getAnimProgress() const {
	if (_openGLModel.isAnimated()) {
		float	timeInTicks = (_animationTime / 1000.0) * _ticksPerSecond;
		return timeInTicks / _curAnimation->mDuration;
	}

	logWarn("the model have no animation");
	return 0;
}

/**
 * @brief get the animation duration in ms
 *
 * @return float, the time in ms
 */
float	Model::getAnimDuration() const {
	if (_openGLModel.isAnimated()) {
		return (_curAnimation->mDuration / _ticksPerSecond) * 1000;
	}

	logWarn("the model have no animation");
	return 0;
}

// -- _updateAnimationTime -----------------------------------------------------
void	Model::_updateAnimationTime() {
	_animationTime += 1000 * _dtTime * animationSpeed;

	if (loopAnimation) {
		_animationTime = fmod(_animationTime, getAnimDuration());
	}
	else {
		_animationTime = std::clamp(_animationTime, 0.0f, getAnimDuration());
	}

	_animationTimeTick = (_animationTime / 1000.0) * _ticksPerSecond;
}

// -- _updateTicksPerSecond ----------------------------------------------------
void	Model::_updateTicksPerSecond() {
	if (_openGLModel.isAnimated()) {
		_ticksPerSecond = _curAnimation->mTicksPerSecond;
		if (_curAnimation->mTicksPerSecond == 0) {
			_ticksPerSecond = 25.0f;
		}
	}
	else {
		logWarn("the model have no animation");
	}
}
