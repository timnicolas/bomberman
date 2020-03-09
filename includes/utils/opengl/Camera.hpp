#ifndef CAMERA_HPP_
#define CAMERA_HPP_

#include "includesOpengl.hpp"

#define CONSTRAINT_Y		false  // if true, move forward didn't affect Y position
#define MOVEMENT_SPEED		15.0f
#define RUN_FACTOR			3.0f
#define MOUSE_SENSITIVITY	0.1f

enum class CamMovement {
	Forward,
	Backward,
	Left,
	Right,
	Up,
	Down
};

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

#define CAMERA_VEC3		glm::vec3
#define CAMERA_MAT4		glm::mat4
#define CAMERA_FLOAT	float

#define WORD_UP			glm::vec3(0.0f, 1.0f, 0.0f)

class Camera {
	public:
		Camera(CAMERA_VEC3 pos = CAMERA_VEC3(0.0f, 0.0f, 0.0f), CAMERA_VEC3 up = \
		CAMERA_VEC3(0.0f, 1.0f, 0.0f), CAMERA_FLOAT yaw = -90.0f, CAMERA_FLOAT pitch = 0.0f);
		Camera(Camera const &src);
		virtual ~Camera();

		Camera &operator=(Camera const &rhs);

		virtual void run(CAMERA_FLOAT dtTime);  // call this function each frame
		CAMERA_MAT4 getViewMatrix() const;
		virtual void processKeyboard(CamMovement direction, CAMERA_FLOAT dtTime, bool isRun = false);
		virtual void processMouseMovement(CAMERA_FLOAT xOffset, CAMERA_FLOAT yOffset, bool constrainPitch = true);
		virtual void processMouseScroll(CAMERA_FLOAT yOffset);
		virtual void lookAt(CAMERA_VEC3 target);
		virtual void resetPosition();

		/*
		check if objects are inside or outside of the camera
		*/
		void	frustumCullingInit(CAMERA_FLOAT angleDeg, CAMERA_FLOAT ratio, CAMERA_FLOAT nearD, CAMERA_FLOAT farD);
		int		frustumCullingCheckPoint(CAMERA_VEC3 const &point);  // check if a point is inside the camera
		int		frustumCullingCheckCube(CAMERA_VEC3 const &startPoint, CAMERA_VEC3 &size);  // check for a cube

		CAMERA_VEC3	pos;
		CAMERA_VEC3	front;
		CAMERA_VEC3	up;
		CAMERA_VEC3	right;
		CAMERA_VEC3	worldUp;

		CAMERA_FLOAT		yaw;
		CAMERA_FLOAT		pitch;

		CAMERA_FLOAT		movementSpeed;
		CAMERA_FLOAT		mouseSensitivity;
		CAMERA_FLOAT		runFactor;
		CAMERA_FLOAT		zoom;

	private:
		void updateCameraVectors();

		/* used to reset position and rotation */
		CAMERA_VEC3	_startPos;
		CAMERA_FLOAT		_startYaw;
		CAMERA_FLOAT		_startPitch;

		struct FrustumCulling {  // frustum culling parameters
			bool	enabled;
			CAMERA_FLOAT	nearD;
			CAMERA_FLOAT	farD;
			CAMERA_FLOAT	width;
			CAMERA_FLOAT	height;
			CAMERA_FLOAT	tang;
			CAMERA_FLOAT	ratio;

			FrustumCulling() : enabled(false) {}
		};
		FrustumCulling	_frustumCulling;
};

#endif  // CAMERA_HPP_
