#pragma once
#include "Matrix4.h"

class RenderProperties
{
public:
	RenderProperties() : projection(Matrix4f()), headlightPos(Vector3f()), headlightTarg(Vector3f()), viewMode(0){};
	Matrix4f projection;
	Vector3f headlightPos;
	Vector3f headlightTarg;
	int viewMode;
};
