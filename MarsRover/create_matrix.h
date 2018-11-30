#pragma once
#include "Matrix4.h"
///class for creating composite transformation matrix and projection matrix
class create_matrix
{
public:
	///Create transformation matrix
	Matrix4<float> create_transformation_matrix(const Vector3<float>& Translation, const float& RotationX, const float& RotationY, const float& Scaling) const;

	Matrix4<float> create_transformation_matrix(const Vector3<float>& Translation, const Matrix4f & rotation, const float & Scaling) const;

	///Create projection matrix
	//Matrix4<float> create_projection(const bool& orthographic, const float& aspect_ratio) const;
};
