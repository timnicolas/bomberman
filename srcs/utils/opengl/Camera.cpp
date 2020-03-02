#include "Camera.hpp"
#include "Logging.hpp"

Camera::Camera(CAMERA_VEC3 pos, CAMERA_VEC3 up, CAMERA_FLOAT yaw, CAMERA_FLOAT pitch)
: pos(pos),
  front(CAMERA_VEC3(0.0f, 0.0f, -1.0f)),
  worldUp(up),
  yaw(yaw),
  pitch(pitch),
  movementSpeed(MOVEMENT_SPEED),
  mouseSensitivity(MOUSE_SENSITIVITY),
  runFactor(RUN_FACTOR),
  zoom(45.0f),
  _startPos(pos),
  _startYaw(yaw),
  _startPitch(pitch) {
	updateCameraVectors();
}

Camera::Camera(Camera const &src) {
	*this = src;
}

Camera::~Camera() {
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
		zoom = rhs.zoom;
	}
	return *this;
}

void	Camera::run(CAMERA_FLOAT dtTime) {
	// process for each frame (gravity)
	(void)dtTime;
}

CAMERA_MAT4 Camera::getViewMatrix() const {
	return glm::lookAt(pos, pos + front, up);
}

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

void	Camera::processMouseMovement(CAMERA_FLOAT xOffset, CAMERA_FLOAT yOffset, bool constrainPitch) {
	xOffset *= mouseSensitivity;
	yOffset *= mouseSensitivity;

	yaw += xOffset;
	pitch += yOffset;

	// constrain pitch to avoid screen flip
	if (constrainPitch) {
		if (pitch > 89.0f)
			pitch = 89.0f;
		if (pitch < -89.0f)
			pitch = -89.0f;
	}

	updateCameraVectors();
}

void	Camera::processMouseScroll(CAMERA_FLOAT yOffset) {
	if (zoom >= 1.0f && zoom <= 45.0f)
		zoom -= yOffset;
	if (zoom <= 1.0f)
		zoom = 1.0f;
	if (zoom >= 45.0f)
		zoom = 45.0f;
}

void	Camera::lookAt(CAMERA_VEC3 target) {
	CAMERA_VEC3	newFront(glm::normalize(target - pos));
	yaw = glm::degrees(glm::atan(newFront.z, newFront.x));
	pitch = glm::degrees(glm::asin(newFront.y));
	updateCameraVectors();
}

void	Camera::updateCameraVectors() {
	CAMERA_VEC3 nFront;

	nFront.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	nFront.y = sin(glm::radians(pitch));
	nFront.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

	front = glm::normalize(nFront);

	right = glm::normalize(glm::cross(front, worldUp));
	up = glm::normalize(glm::cross(right, front));
}

void	Camera::resetPosition() {
	pos = _startPos;
	yaw = _startYaw;
	pitch = _startPitch;

	updateCameraVectors();
}

/*
init the frustum culling (take the same args as projection matrix)
*/
void	Camera::frustumCullingInit(CAMERA_FLOAT angleDeg, CAMERA_FLOAT ratio, CAMERA_FLOAT nearD, CAMERA_FLOAT farD) {
	_frustumCulling.tang = static_cast<CAMERA_FLOAT>(glm::tan(glm::radians(angleDeg * 0.5)));

	_frustumCulling.ratio = ratio;
	_frustumCulling.nearD = nearD;
	_frustumCulling.farD = farD;

	_frustumCulling.height = _frustumCulling.nearD * _frustumCulling.tang;
	_frustumCulling.width = _frustumCulling.height * _frustumCulling.ratio;

	_frustumCulling.enabled = true;
}

/*
check if a point is inside or outside the camera
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

/*
check if a cube is inside or outside of the camera
startPoint is the 0|0|0 coordinate of the cube
size if the scale in X Y and Z
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
