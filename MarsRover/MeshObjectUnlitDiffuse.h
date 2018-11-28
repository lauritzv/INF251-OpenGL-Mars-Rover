#pragma once
#include "MeshObject.h"
class MeshObjectUnlitDiffuse :
	public MeshObject
{
public:
	MeshObjectUnlitDiffuse(const char* modelpath, MaterialObject& material_object, GLuint& sp);
	~MeshObjectUnlitDiffuse();
	void DrawObject(Matrix4f &transf, Matrix4f &projection) const override;

protected:
	GLuint &TObjectDiff;
	Vector3f tintColor;
};
