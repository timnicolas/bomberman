#include "Camera.hpp"
#include "Logging.hpp"
#include "Inputs.hpp"

// -- Constructors -------------------------------------------------------------
Camera::Camera(float const ratio, CAMERA_VEC3 pos, CAMERA_VEC3 up, CAMERA_FLOAT yaw,
	CAMERA_FLOAT pitch)
: pos(pos),
  front(CAMERA_VEC3(0.0f, 0.0f, -1.0f)),
  worldUp(up),
  yaw(yaw),
  pitch(pitch),
  movementSpeed(MOVEMENT_SPEED),
  mouseSensitivity(MOUSE_SENSITIVITY),
  runFactor(RUN_FACTOR),
  _mode(CamMode::FPS),
  _ratio(ratio),
  _startPos(pos),
  _startYaw(yaw),
  _startPitch(pitch)
{
	_fovY = 45.0f;
	_near = 0.1f;
	_far = 1000;
	_updateProjection();

	_updateCameraVectors();
}

Camera::~Camera() {
}

Camera::Camera(Camera const &src) {
	*this = src;
}

Camera	&Camera::operator=(Camera const &rhs) {
	if (this != &rhs) {
		pos = rhs.pos;
		front = rhs.front;
		up = rhs.up;
		right = rhs.right;
		worldUp = rhs.worldUp;
		yaw = rhs.yaw;
		pitch = rhs.pitch;
		movementSpeed = rhs.movementSpeed;
		mouseSensitivity = rhs.mouseSensitivity;
		_ratio = rhs._ratio;
		_fovY = rhs._fovY;
		_near = rhs._near;
		_far = rhs._far;
		_projection = rhs._projection;
	}
	return *this;
}

// -- lookAt -------------------------------------------------------------------
/**
 * @brief Move the camera orientation to look at the target
 *
 * @param target The target to look at
 */
void	Camera::lookAt(CAMERA_VEC3 target) {
	CAMERA_VEC3	newFront(glm::normalize(target - pos));
	yaw = glm::degrees(glm::atan(newFront.z, newFront.x));
	pitch = glm::degrees(glm::asin(newFront.y));
	_updateCameraVectors();
}

// -- setters ------------------------------------------------------------------
/**
 * @brief Reset the camera position
 */
void	Camera::resetPosition() {
	pos = _startPos;
	yaw = _startYaw;
	pitch = _startPitch;

	_updateCameraVectors();
}

void	Camera::setRatio(float ratio) {
	_ratio = ratio;
	_updateProjection();
}

void	Camera::setFovY(float fovY) {
	_fovY = fovY;
	_updateProjection();
}

void	Camera::setNearAndFar(float near, float far) {
	if (near > far) {
		logErr("near should be < than far");
		return;
	}
	_near = near;
	_far = far;
	_updateProjection();
}

/**
 * @brief Update the current camera mode
 *
 * @param mode CamMode element
 */
void	Camera::setMode(CamMode::Enum mode) {
	if (mode == _mode)
		return;
	_mode = mode;
}

/**
 * @brief Set the default camera position
 *
 * @param defPos Default position
 * @param defYaw Default yaw
 * @param defPitch Default pitch
 */
void	Camera::setDefPos(CAMERA_VEC3 defPos, CAMERA_FLOAT defYaw, CAMERA_FLOAT defPitch) {
	_startPos = defPos;
	_startYaw = defYaw;
	_startPitch = defPitch;
}
/**
 * @brief Set the default camera position to the current position
 */
void	Camera::setDefPos() {
	_startPos = pos;
	_startYaw = yaw;
	_startPitch = pitch;
}

// -- update -------------------------------------------------------------------
/**
 * @brief Update function of the camera -> to be called every frames
 *
 * @param dtTime The delta time
 */
void	Camera::update(float dtTime) {
	switch (_mode) {
		case CamMode::STATIC_DEFPOS:
			resetPosition();
			_updateStatic(dtTime);
			break;
		case CamMode::STATIC:
			_updateStatic(dtTime);
			break;
		case CamMode::FPS:
			_updateFps(dtTime);
			break;
		case CamMode::FOLLOW_PATH:
			_updateFollowPath(dtTime);
			break;
	}
}

void	Camera::_updateStatic(float dtTime) {
	(void)dtTime;
	// nothing to do in static camera update
}
void	Camera::_updateFps(float dtTime) {
	// mouse movement
	processMouseMovement(Inputs::getMouseRel());

	bool isRun = false;
	if (Inputs::getKeyByScancode(SDL_SCANCODE_LSHIFT)) {
		isRun = true;
	}

	// camera movement
	if (Inputs::getKeyByScancode(SDL_SCANCODE_W)) {
		processKeyboard(CamMovement::Forward, dtTime, isRun);
	}
	if (Inputs::getKeyByScancode(SDL_SCANCODE_D)) {
		processKeyboard(CamMovement::Right, dtTime, isRun);
	}
	if (Inputs::getKeyByScancode(SDL_SCANCODE_S)) {
		processKeyboard(CamMovement::Backward, dtTime, isRun);
	}
	if (Inputs::getKeyByScancode(SDL_SCANCODE_A)) {
		processKeyboard(CamMovement::Left, dtTime, isRun);
	}
	if (Inputs::getKeyByScancode(SDL_SCANCODE_Q)) {
		processKeyboard(CamMovement::Down, dtTime, isRun);
	}
	if (Inputs::getKeyByScancode(SDL_SCANCODE_E)) {
		processKeyboard(CamMovement::Up, dtTime, isRun);
	}
}
void	Camera::_updateFollowPath(float dtTime) {
	(void)dtTime;
}

// -- processKeyboard ----------------------------------------------------------
/**
 * @brief Move camera from keyboard interaction
 *
 * @param direction The direction to move (CamMovement::Forward, CamMovement::Left, ...)
 * @param dtTime The delta time since last call
 * @param isRun Option to know is the player is running or not
 */
void	Camera::processKeyboard(CamMovement direction, CAMERA_FLOAT dtTime, bool isRun) {
	CAMERA_FLOAT	velocity;

	velocity = movementSpeed * dtTime * ((isRun) ? runFactor : 1);
	if (direction == CamMovement::Forward) {
		#if CONSTRAINT_Y == true

			CAMERA_VEC3 tmpFront = front;
			tmpFront.y = 0;
			tmpFront = glm::normalize(tmpFront);
			pos = pos + tmpFront * velocity;
		#else
			pos = pos + front * velocity;
		#endif
	}
	if (direction == CamMovement::Backward) {
		#if CONSTRAINT_Y == true
			CAMERA_VEC3 tmpFront = front;
			tmpFront.y = 0;
			tmpFront = glm::normalize(tmpFront);
			pos = pos - tmpFront * velocity;
		#else
			pos = pos - front * velocity;
		#endif
	}
	if (direction == CamMovement::Left) {
		pos = pos - right * velocity;
	}
	if (direction == CamMovement::Right) {
		pos = pos + right * velocity;
	}
	if (direction == CamMovement::Up) {
		pos = pos + worldUp * velocity;
	}
	if (direction == CamMovement::Down) {
		pos = pos - worldUp * velocity;
	}
}

// -- processMouseMovement -----------------------------------------------------
/**
 * @brief Move the camera from the mouse movements
 *
 * @param xOffset The camera offset in x from last call
 * @param yOffset The camera offset in y from last call
 * @param constrainPitch Constrain pitch to block camera up to 90 deg (enable by default)
 */
void	Camera::processMouseMovement(glm::vec2 offset, bool constrainPitch) {
	offset.y = -offset.y;
	offset *= mouseSensitivity;

	yaw += offset.x;
	pitch += offset.y;

	// constrain pitch to avoid screen flip
	if (constrainPitch) {
		if (pitch > 89.0f)
			pitch = 89.0f;
		if (pitch < -89.0f)
			pitch = -89.0f;
	}

	_updateCameraVectors();
}

// -- frustumCullingInit -------------------------------------------------------
/**
 * @brief init the frustum culling (take the same args as projection matrix)
 *
 * @param angleDeg The projection angle (in degrees)
 * @param ratio The ratio (width / height)
 * @param nearD The near distance (don't show before this distance)
 * @param farD The far distance (don't show after this distance)
 */
void	Camera::frustumCullingInit(CAMERA_FLOAT angleDeg, CAMERA_FLOAT ratio,
	CAMERA_FLOAT nearD, CAMERA_FLOAT farD)
{
	_frustumCulling.tang = static_cast<CAMERA_FLOAT>(glm::tan(
		glm::radians(angleDeg * 0.5)));

	_frustumCulling.ratio = ratio;
	_frustumCulling.nearD = nearD;
	_frustumCulling.farD = farD;

	_frustumCulling.height = _frustumCulling.nearD * _frustumCulling.tang;
	_frustumCulling.width = _frustumCulling.height * _frustumCulling.ratio;

	_frustumCulling.enabled = true;
}

// -- frustumCullingCheckPoint -------------------------------------------------
/**
 * @brief Check if a point is inside or outside the camera
 *
 * @param point The 3D point
 * @return int The point position (FRCL_INSIDE == is inside)
 */
int		Camera::frustumCullingCheckPoint(CAMERA_VEC3 const &point) {
	CAMERA_FLOAT	pcz, pcx, pcy, aux;
	int		res = FRCL_INSIDE;

	if (!_frustumCulling.enabled) {
		logWarn("init frustumCulling before using it: frustumCullingInit");
		return FRCL_INSIDE;
	}

	// compute vector from camera position to p
	CAMERA_VEC3 point2pos = point - pos;

	// compute and test the Z coordinate to get distance from camera
	pcz = glm::dot(point2pos, front);
	if (pcz > _frustumCulling.farD)
		res |= FRCL_FAR;
	else if (pcz < _frustumCulling.nearD)
 		res |= FRCL_NEAR;

 	// compute and test the Y coordinate to get distance from camera
 	pcy = glm::dot(point2pos, up);
 	aux = pcz * _frustumCulling.tang;
 	if (pcy > aux)
		res |= FRCL_TOP;
	else if (pcy < -aux)
 		res |= FRCL_BOTTOM;

 	// compute and test the X coordinate to get distance from camera
 	pcx = glm::dot(point2pos, right);
 	aux = aux * _frustumCulling.ratio;
 	if (pcx > aux)
		res |= FRCL_RIGHT;
	else if (pcx < -aux)
 		res |= FRCL_LEFT;

	return res;
}

// -- frustumCullingCheckCube --------------------------------------------------
/**
 * @brief Check if a cube or rectangle is inside or outside of the camera
 *
 * @param startPoint The 0|0|0 coordinate of the cube
 * @param size The scale in X Y and Z of the cube
 * @return int The point position (FRCL_INSIDE == is inside)
 */
int		Camera::frustumCullingCheckCube(CAMERA_VEC3 const &startPoint, CAMERA_VEC3 &size) {
	int			res;  // point1 & point2 & point3 ...
	int			tmpRes;
	CAMERA_VEC3	pos;

	// compute on all positions
	pos = startPoint;
	tmpRes = frustumCullingCheckPoint(pos);
	if (tmpRes == FRCL_INSIDE) return FRCL_INSIDE;
	res = tmpRes;

	pos = startPoint;
	pos.x += size.x;
	tmpRes = frustumCullingCheckPoint(pos);
	if (tmpRes == FRCL_INSIDE) return FRCL_INSIDE;
	res &= tmpRes;

	pos = startPoint;
	pos.z += size.z;
	tmpRes = frustumCullingCheckPoint(pos);
	if (tmpRes == FRCL_INSIDE) return FRCL_INSIDE;
	res &= tmpRes;

	pos = startPoint;
	pos.x += size.x;
	pos.z += size.z;
	tmpRes = frustumCullingCheckPoint(pos);
	if (tmpRes == FRCL_INSIDE) return FRCL_INSIDE;
	res &= tmpRes;

	pos = startPoint;
	pos.y += size.y;
	tmpRes = frustumCullingCheckPoint(pos);
	if (tmpRes == FRCL_INSIDE) return FRCL_INSIDE;
	res &= tmpRes;

	pos = startPoint;
	pos.x += size.x;
	pos.y += size.y;
	tmpRes = frustumCullingCheckPoint(pos);
	if (tmpRes == FRCL_INSIDE) return FRCL_INSIDE;
	res &= tmpRes;

	pos = startPoint;
	pos.y += size.y;
	pos.z += size.z;
	tmpRes = frustumCullingCheckPoint(pos);
	if (tmpRes == FRCL_INSIDE) return FRCL_INSIDE;
	res &= tmpRes;

	pos = startPoint;
	pos.x += size.x;
	pos.y += size.y;
	pos.z += size.z;
	tmpRes = frustumCullingCheckPoint(pos);
	if (tmpRes == FRCL_INSIDE) return FRCL_INSIDE;
	res &= tmpRes;

	if (res > 0)
		return FRCL_OUTSIDE;  // outside
	else
		return FRCL_INSIDE;
}

// -- _updateCameraVectors -----------------------------------------------------
/**
 * @brief Update the camera after moving or changing orientation
 */
void	Camera::_updateCameraVectors() {
	CAMERA_VEC3 nFront;

	nFront.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	nFront.y = sin(glm::radians(pitch));
	nFront.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

	front = glm::normalize(nFront);

	right = glm::normalize(glm::cross(front, worldUp));
	up = glm::normalize(glm::cross(right, front));
}

// -- _updateProjection --------------------------------------------------------
void	Camera::_updateProjection() {
	_projection = glm::perspective(glm::radians(_fovY), _ratio, _near, _far);
}

// -- getters ------------------------------------------------------------------
CAMERA_MAT4 Camera::getViewMatrix() const {
	return glm::lookAt(pos, pos + front, up);
}

CAMERA_MAT4	Camera::getProjection() const { return _projection; }

CamMode::Enum	Camera::getMode() const { return _mode; }
