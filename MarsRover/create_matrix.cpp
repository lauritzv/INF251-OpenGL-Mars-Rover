#include "create_matrix.h"
Matrix4<float> create_matrix::create_transformation_matrix(const Vector3<float>& Translation, const float& RotationX, const float& RotationY, const float& Scaling) const
{
	const auto translation_mat = Matrix4<float>().createTranslation(Translation);  // NOLINT
	const auto rotation_x_mat = Matrix4<float>().createRotation(RotationX, Vector3f(0, 1, 0)); // NOLINT
	const auto rotation_y_mat = Matrix4<float>().createRotation(RotationY, Vector3f(-1, 0, 0)); // NOLINT
	const auto scaling_mat = Matrix4<float>().createScaling(Scaling, Scaling, Scaling); // NOLINT

	return translation_mat * rotation_y_mat * rotation_x_mat * scaling_mat;
}

Matrix4<float> create_matrix::create_transformation_matrix(const Vector3<float>& Translation, const Matrix4f& rotation, const float& Scaling) const
{
	const auto translation_mat = Matrix4<float>().createTranslation(Translation);  // NOLINT
	const auto scaling_mat = Matrix4<float>().createScaling(Scaling, Scaling, Scaling); // NOLINT

	return translation_mat * (rotation * scaling_mat);
}

//Matrix4<float> create_matrix::create_projection(const bool& orthographic, const float& aspect_ratio) const
//{
//	Matrix4<float> ret;
//	if (orthographic)
//		ret = Matrix4<float>().createOrthoPrj(-5.*aspect_ratio, 5.*aspect_ratio, -5., 5., 0.1, 15.); // NOLINT
//	else ret = Matrix4<float>().createPerspectivePrj(90.0f, aspect_ratio, 0.1f, 1000.0f); // NOLINT
//	return ret;
//}
