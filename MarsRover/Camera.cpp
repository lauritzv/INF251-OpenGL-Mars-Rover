#include "Camera.h"

Matrix4f Camera::computeCameraTransform() const
{
	// camera rotation
	Vector3f t = target.getNormalized();
	Vector3f u = up.getNormalized();
	Vector3f r = t.cross(u);
	const Matrix4f camR(
		r.x(), r.y(), r.z(), 0.f,
		u.x(), u.y(), u.z(), 0.f,
		-t.x(), -t.y(), -t.z(), 0.f,
		0.f, 0.f, 0.f, 1.f
	);

	// camera translation
	Matrix4f camT = Matrix4f::createTranslation(-position);

	// perspective projection
	Matrix4f prj = Matrix4f::createPerspectivePrj(fov, ar, zNear, zFar);

	// scaling due to zooming
	Matrix4f camZoom = Matrix4f::createScaling(zoom, zoom, 1.f);

	// Final transformation. Notice the multiplication order
	// First vertices are moved in camera space
	// Then the perspective projection puts them in clip space
	// And a final zooming factor is applied in clip space
	return camZoom * prj * camR  * camT;

} /* computeCameraTransform() */

void Camera::init()
{
	position.set(0.f, 0.f, 0.f);
	target.set(0.f, 0.f, -1.f);
	//target.set(0.0f, 0.0f, -10.0f);
	up.set(0.f, 1.f, 0.f);
	fov = 50.f;
	ar = 1.f; // will be correctly initialized in the "display()" method
	zNear = 0.1f;
	zFar = 100.f;
	zoom = 1.f;
}