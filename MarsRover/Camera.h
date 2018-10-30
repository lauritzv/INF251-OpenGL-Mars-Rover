#pragma once
#include "Vector3.h"
#include "Matrix4.h"

class Camera
{
public:
	Vector3f position;	///< the position of the camera
	Vector3f target;	///< the direction the camera is looking at
	Vector3f up;		///< the up vector of the camera

	float fov;			///< camera field of view
	float ar;			///< camera aspect ratio
	float zNear, zFar;	///< depth of the near and far plane
	float zoom;			///< an additional scaling parameter

	Matrix4f computeCameraTransform() const;
	void init();
};
