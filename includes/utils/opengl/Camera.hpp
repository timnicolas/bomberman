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

/**
 * @brief All camera base movements
 */
enum class CamMovement {
	Forward,
	Backward,
	Left,
	Right,
	Up,
	Down,
	NoDirection,
};

namespace CamMode {
	/**
	 * @brief Camera modes list
	 */
	enum Enum {
		STATIC_DEFPOS,  /**< impossible to move, camera is always to the default position */
		STATIC,  /**< impossible to move */
		FPS,  /**< moving using keyboard and mouse */
		FOLLOW_PATH,  /**< moving by follow a path */
	};
}  // namespace CamMode

/**
 * @brief Point on the word (used in FOLLOW_PATH mode)
 */
struct CamPoint {
	CAMERA_VEC3	pos;  /**< position */
	CamMovement	lookDir;  /**< looking direction (if NoDirection -> look at CamPoint::lookAt) */
	CAMERA_VEC3	lookAt;  /**< lookAt position for last point to pos */
	bool		tpTo;  /**< if true, tp to the position */
	float		speed;  /**< speed to move to this point (if <= 0 -> default speed) */
};

/**
 * @brief struct to store frustum culling settings
 */
struct FrustumCulling {
	bool	enabled;  /**< frustum culling enabled or disabled */
	CAMERA_FLOAT	nearD;  /**< frustum culling near */
	CAMERA_FLOAT	farD;  /**< frustum culling far */
	CAMERA_FLOAT	width;  /**< frustum culling width */
	CAMERA_FLOAT	height;  /**< frustum culling height */
	CAMERA_FLOAT	tang;  /**< frustum culling tangent */
	CAMERA_FLOAT	ratio;  /**< frustum culling ratio */

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
		int		frustumCullingCheckPoint(CAMERA_VEC3 const &point) const;  // check if a point is inside the camera
		int		frustumCullingCheckCube(CAMERA_VEC3 const &startPoint, CAMERA_VEC3 const &size) const;  // check for a cube
		int		frustumCullingCheckSphere(CAMERA_VEC3 const &center, CAMERA_FLOAT radius) const;  // check for a sphere

		/* follow path */
		void	resetFollowPath();
		void	setFollowPath(std::vector<CamPoint> const & path);
		void	setFollowRepeat(bool repeat);
		bool	isFollowFinished() const;
		bool	isFollowRepeat() const;

		// getters
		CAMERA_MAT4		getViewMatrix() const;
		CAMERA_MAT4		getProjection() const;
		CamMode::Enum	getMode() const;
		CAMERA_VEC3		getDefPos() const;

		CAMERA_VEC3		pos;  /**< Camera position */
		CAMERA_VEC3		front;  /**< Camera front vector */
		CAMERA_VEC3		up;  /**< Camera up vector */
		CAMERA_VEC3		right;  /**< Camera right vector */
		CAMERA_VEC3		worldUp;  /**< Global world up vector (0, 1, 0) */

		CAMERA_FLOAT	yaw;  /**< Camera yaw */
		CAMERA_FLOAT	pitch;  /**< Camera pitch */

		CAMERA_FLOAT	movementSpeed;  /**< Camera speed */
		CAMERA_FLOAT	mouseSensitivity;  /**< Camera moving sensitivity */
		CAMERA_FLOAT	runFactor;  /**< Running factor (movementSpeed * runFactor) */


	private:
		void	_updateCameraVectors();
		void	_updateProjection();

		/* update */
		void	_updateStatic(float dtTime);
		void	_updateFps(float dtTime);
		void	_updateFollowPath(float dtTime);

		CamMode::Enum	_mode;  /**< Camera mode (STATIC, FPS, ...) */
		float			_ratio;  /**< Camera ratio */
		float			_fovY;  /**< Camera fov */
		float			_near;  /**< Camera near (cannot see before near) */
		float			_far;  /**< Camera far (cannot see after far) */
		CAMERA_MAT4		_projection;  /**< Camera projection matrix */

		/* used to reset position and rotation */
		CAMERA_VEC3		_startPos;  /**< Starting position */
		CAMERA_FLOAT	_startYaw;  /**< Starting yaw */
		CAMERA_FLOAT	_startPitch;  /**< Starting pitch */

		/* for follow path mode */
		bool					_followIsRepeat;  /**< For follow mode, repeat after follow */
		bool					_followIsFinished;  /**< For follow mode, true when follow is finished */
		std::vector<CamPoint>	_followPath;  /**< For follow mode, the path to follow */
		int						_followCurElem;  /**< For follow mode, the current following element */

		FrustumCulling	_frustumCulling;  /**< The frustum culling object */
};

#endif  // CAMERA_HPP_
