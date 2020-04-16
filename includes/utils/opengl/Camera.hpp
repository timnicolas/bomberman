#ifndef CAMERA_HPP_
#define CAMERA_HPP_

#include <vector>
#include "includesOpengl.hpp"

#define CONSTRAINT_Y		false  // if true, move forward didn't affect Y position
#define MOVEMENT_SPEED		15.0f
#define RUN_FACTOR			3.0f
#define MOUSE_SENSITIVITY	0.1f

/* frustum culling parameter */
#define FRCL_INSIDE		0b000000  // inside the camera
#define FRCL_LEFT		0b000001  // outside left to the camera
#define FRCL_RIGHT		0b000010  // outside right to the camera
#define FRCL_TOP		0b000100  // outside top to the camera
#define FRCL_BOTTOM		0b001000  // outside bottom to the camera
#define FRCL_NEAR		0b010000  // outside near to the camera
#define FRCL_FAR		0b100000  // outside far to the camera
#define FRCL_OUTSIDE	0b111111  // outside form unknow side

#define FRCL_IS_INSIDE(frustumRes) (frustumRes == FRCL_INSIDE)
#define FRCL_IS_OUTSIDE(frustumRes) (frustumRes > FRCL_INSIDE)

/* camera using float or double */
#define CAMERA_VEC3		glm::vec3
#define CAMERA_MAT4		glm::mat4
#define CAMERA_FLOAT	float

/* def up vector */
#define WORD_UP			glm::vec3(0.0f, 1.0f, 0.0f)

/* all camera base movements */
enum class CamMovement {
	Forward,
	Backward,
	Left,
	Right,
	Up,
	Down,
	NoDirection,
};

/* camera modes list */
namespace CamMode {
	enum Enum {
		STATIC_DEFPOS,  // impossible to move, camera is always to the default position
		STATIC,  // impossible to move
		FPS,  // moving using keyboard and mouse
		FOLLOW_PATH,  // moving by follow a path
	};
}

/* point on the word (used in FOLLOW_PATH mode) */
struct CamPoint {
	CAMERA_VEC3	pos;
	CamMovement	lookDir;  // looking direction (if NoDirection -> look at CamPoint::lookAt)
	CAMERA_VEC3	lookAt;  // lookAt position for last point to pos
	bool		tpTo;  // if true, tp to the position
	float		speed;  // speed to move to this point (if <= 0 -> default speed)
};

/**
 * @brief struct to store frustum culling settings
 *
 */
struct FrustumCulling {
	bool	enabled;
	CAMERA_FLOAT	nearD;
	CAMERA_FLOAT	farD;
	CAMERA_FLOAT	width;
	CAMERA_FLOAT	height;
	CAMERA_FLOAT	tang;
	CAMERA_FLOAT	ratio;

	FrustumCulling() : enabled(false) {}
};

/**
 * @brief this is a camera 3D object
 *
 * this object implements functions to:
 *  - move with keyboard (processKeyboard)
 *  - turn camera with mouse (processMouseMovement)
 *  - look at a point (lookAt)
 *  - know if a 3D point or a 3D rectangle is inside the camera angle (frustumCulling...)
 */
class Camera {
	public:
		Camera(float const ratio, CAMERA_VEC3 pos = CAMERA_VEC3(0.0f, 0.0f, 0.0f),
			CAMERA_VEC3 up = CAMERA_VEC3(0.0, 1.0, 0.0), CAMERA_FLOAT yaw = -90.0f,
			CAMERA_FLOAT pitch = 0.0f);
		virtual ~Camera();
		Camera(Camera const &src);
		Camera &operator=(Camera const &rhs);

		void	lookAt(CAMERA_VEC3 target);

		// setters
		void	resetPosition();
		void	setRatio(float ratio);
		void	setFovY(float fovY);
		void	setNearAndFar(float near, float far);
		void	setMode(CamMode::Enum mode);
		void	setDefPos(CAMERA_VEC3 defPos, CAMERA_FLOAT defYaw = -90, CAMERA_FLOAT defPitch = 0);
		void	setDefPos();

		// update
		void	update(float dtTime);

		// to manage basic camera fly movement
		void	processKeyboard(CamMovement direction, CAMERA_FLOAT dtTime, bool isRun = false);
		void	processMouseMovement(glm::vec2 offset, bool constrainPitch = true);

		/* frustum culling */
		// to test if objects are inside or outside of the camera
		void	frustumCullingInit(CAMERA_FLOAT angleDeg, CAMERA_FLOAT ratio, CAMERA_FLOAT nearD, CAMERA_FLOAT farD);
		int		frustumCullingCheckPoint(CAMERA_VEC3 const &point);  // check if a point is inside the camera
		int		frustumCullingCheckCube(CAMERA_VEC3 const &startPoint, CAMERA_VEC3 &size);  // check for a cube

		/* follow path */
		void	setFollowPath(std::vector<CamPoint> const & path);
		void	setFollowRepeat(bool repeat);
		void	setFollowSpeed(float speed_);
		bool	isFollowFinished() const;
		bool	isFollowRepeat() const;

		// getters
		CAMERA_MAT4		getViewMatrix() const;
		CAMERA_MAT4		getProjection() const;
		CamMode::Enum	getMode() const;
		CAMERA_VEC3		getDefPos() const;

		CAMERA_VEC3		pos;
		CAMERA_VEC3		front;
		CAMERA_VEC3		up;
		CAMERA_VEC3		right;
		CAMERA_VEC3		worldUp;

		CAMERA_FLOAT	yaw;
		CAMERA_FLOAT	pitch;

		CAMERA_FLOAT	movementSpeed;
		CAMERA_FLOAT	mouseSensitivity;
		CAMERA_FLOAT	runFactor;


	private:
		void	_updateCameraVectors();
		void	_updateProjection();

		/* update */
		void	_updateStatic(float dtTime);
		void	_updateFps(float dtTime);
		void	_updateFollowPath(float dtTime);

		CamMode::Enum	_mode;  // STATIC, FPS, ...
		float			_ratio;
		float			_fovY;
		float			_near;
		float			_far;
		CAMERA_MAT4		_projection;

		/* used to reset position and rotation */
		CAMERA_VEC3		_startPos;
		CAMERA_FLOAT	_startYaw;
		CAMERA_FLOAT	_startPitch;

		/* for follow path mode */
		bool					_followIsRepeat;
		bool					_followIsFinished;
		std::vector<CamPoint>	_followPath;
		int						_followCurElem;

		FrustumCulling	_frustumCulling;
};

#endif  // CAMERA_HPP_
